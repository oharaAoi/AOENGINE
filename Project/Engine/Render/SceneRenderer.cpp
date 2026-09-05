#include "SceneRenderer.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <format>
#include <iterator>

#include "Engine/Core/Engine.h"
#include "Engine/Lib/Math/Frustum.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/Materials/PBRMaterial.h"
#include "Engine/Render/Render.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/Manager/ParticleEffectManager.h"

using namespace AOENGINE;

namespace {

std::string SelectRenderingName(const std::string& shader) {
	if (shader == "PBR") {
		return "Object_PBR.json";
	}

	return "Object_Normal.json";
}

MaterialType SelectMaterialType(const std::string& shader) {
	if (shader == "PBR") {
		return MaterialType::PBR;
	}

	return MaterialType::Normal;
}

bool IntersectTriangle(
	const Math::Vector3& rayOrigin,
	const Math::Vector3& rayDirection,
	const Math::Vector3& vertex0,
	const Math::Vector3& vertex1,
	const Math::Vector3& vertex2,
	float& distance) {
	constexpr float kRayEpsilon = 1.0e-6f;
	const Math::Vector3 edge1 = vertex1 - vertex0;
	const Math::Vector3 edge2 = vertex2 - vertex0;
	const Math::Vector3 p = Math::Vector3::Cross(rayDirection, edge2);
	const float determinant = Math::Vector3::Dot(edge1, p);
	if (std::abs(determinant) < kRayEpsilon) { return false; }

	const float inverseDeterminant = 1.0f / determinant;
	const Math::Vector3 t = rayOrigin - vertex0;
	const float u = Math::Vector3::Dot(t, p) * inverseDeterminant;
	if (u < 0.0f || u > 1.0f) { return false; }

	const Math::Vector3 q = Math::Vector3::Cross(t, edge1);
	const float v = Math::Vector3::Dot(rayDirection, q) * inverseDeterminant;
	if (v < 0.0f || u + v > 1.0f) { return false; }

	distance = Math::Vector3::Dot(edge2, q) * inverseDeterminant;
	return distance >= 0.0f;
}

}

SceneRenderer* AOENGINE::SceneRenderer::GetInstance() {
	static SceneRenderer instance;
	return &instance;
}

void SceneRenderer::Finalize() {
	ClearSceneObjects();
	modelInstancingRenderer_.Finalize();
	ParticleEffectManager::GetInstance()->Finalize();

	if (particleManager_) {
		particleManager_->Finalize();
	}
	if (gpuParticleManager_) {
		gpuParticleManager_->Finalize();
	}

	// 通常フレームではEngine::EndFrame()のGPU同期後に回収されるが、
	// 終了処理ではこの後にEndFrameが存在しない。ここで明示的に回収し、
	// D3D12 Deviceより先にScene/Particle配下のResourceを解放する。
	ReleaseRetiredObjects();
}

void SceneRenderer::ClearSceneObjects() {
	renderEntries_.clear();
	sceneWorld_.Clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::Init() {
	ClearSceneObjects();

	particleManager_ = AOENGINE::ParticleManager::GetInstance();
	particleManager_->Init();

	gpuParticleManager_ = AOENGINE::GpuParticleManager::GetInstance();
	gpuParticleManager_->Init();

	RegisterLightObjects();
	AOENGINE::EditorWindows::GetInstance()->SetSceneRenderer(this);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::Update() {
	RemoveInvalidRenderEntries();

	std::stable_sort(renderEntries_.begin(), renderEntries_.end(), [](const RenderEntry& a, const RenderEntry& b) {
		if (a.renderQueue == b.renderQueue) {
			return a.renderingType < b.renderingType;
		}
		return a.renderQueue < b.renderQueue;
	});

	sceneWorld_.Update();
	RemoveInvalidRenderEntries();
}

void SceneRenderer::ReleaseRetiredObjects() {
	sceneWorld_.ReleaseRetiredObjects();
	if (particleManager_) { particleManager_->ReleaseRetiredResources(); }
	if (gpuParticleManager_) { gpuParticleManager_->ReleaseRetiredResources(); }
}

void SceneRenderer::UpdateVerticalPhysics() {
	for (const ObjectHandle& handle : sceneWorld_.GetObjectHandles()) {
		SceneObject* object = sceneWorld_.FindObject(handle);
		if (!object || !object->IsActive()) { continue; }
		if (BaseGameObject* gameObject = dynamic_cast<BaseGameObject*>(object)) {
			gameObject->UpdateVerticalPhysics();
		}
	}
}

void SceneRenderer::ApplyCollisionPushback() {
	for (const ObjectHandle& handle : sceneWorld_.GetObjectHandles()) {
		SceneObject* object = sceneWorld_.FindObject(handle);
		if (!object || !object->IsActive()) { continue; }
		if (BaseGameObject* gameObject = dynamic_cast<BaseGameObject*>(object)) {
			gameObject->ApplyCollisionPushback();
		}
	}
}

void SceneRenderer::EditorUpdate() {
	RemoveInvalidRenderEntries();
	for (const ObjectHandle& handle : sceneWorld_.GetObjectHandles()) {
		SceneObject* object = sceneWorld_.FindObject(handle);
		if (!object || !object->IsActive()) { continue; }
		if (BaseGameObject* gameObject = dynamic_cast<BaseGameObject*>(object)) {
			gameObject->EditorUpdate();
		} else if (Sprite* sprite = dynamic_cast<Sprite*>(object)) {
			sprite->Update();
		}
	}
}

void SceneRenderer::PostUpdate() {
	sceneWorld_.PostUpdate();
	RemoveInvalidRenderEntries();

	// particleの更新
	if (particleManager_->IsActive()) { particleManager_->Update(); }
	if (gpuParticleManager_->IsActive()) { gpuParticleManager_->Update(); }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::Draw() const {
	// 影の描画
	DrawShadowMap();
	// オブジェクトの描画
	DrawSceneObjects();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 影の描画
///////////////////////////////////////////////////////////////////////////////////////////////
void SceneRenderer::DrawShadowMap() const {
	// 1フレーム中のGame/Editor両Viewで異なるUpload Bufferを使う。
	// Viewごとにリセットすると、後から描画するEditor ViewがGame Viewのデータを上書きする。
	modelInstancingRenderer_.BeginFrame();
	AOENGINE::Render::SetShadowMap();
	for (const RenderEntry& entry : renderEntries_) {
		const ISceneObject* obj = GetRenderableObject(entry);
		if (obj && obj->IsActive()) {
			obj->PreDraw();
		}
	}
	AOENGINE::Render::ChangeShadowMap();
}

void SceneRenderer::DrawSceneObjects() const {
	DrawSceneObjects(AOENGINE::Render::GetViewProjectionMat());
}

void SceneRenderer::DrawSceneObjects(const Math::Matrix4x4& viewProjection) const {
	const Math::Frustum cameraFrustum = Math::Frustum::FromViewProjection(viewProjection);

	// 通常3Dモデルは可能な限りInstancing batchへ集約し、最後にまとめて描画します。
	std::vector<AOENGINE::ModelInstancingRenderer::NormalBatch> normalInstancingBatches;

	for (const RenderEntry& entry : renderEntries_) {
		if (entry.isPostDraw) {
			continue;
		}

		const ISceneObject* obj = GetRenderableObject(entry);
		if (obj && obj->IsActive() && IsVisible(*obj, cameraFrustum)) {
			const BaseGameObject* baseObject = dynamic_cast<const BaseGameObject*>(obj);

			// Instancing化できたObjectはここでは描画せず、batch描画へ回します。
			if (baseObject && TryAddNormalInstancingBatch(entry, *baseObject, normalInstancingBatches)) {
				continue;
			}

			// Instancing対象外のObjectは従来通り個別に描画します。
			Engine::SetPipeline(PSOType::Object3d, entry.renderingType);
			obj->Draw();
		}
	}

	modelInstancingRenderer_.DrawNormalBatches(normalInstancingBatches);

	// particleの描画
	if (particleManager_->IsActive()) { particleManager_->Draw(cameraFrustum); }
	if (gpuParticleManager_->IsActive()) { gpuParticleManager_->Draw(cameraFrustum); }
}

void SceneRenderer::PostDraw() const {
	const Math::Frustum cameraFrustum = Math::Frustum::FromViewProjection(AOENGINE::Render::GetViewProjectionMat());

	for (const RenderEntry& entry : renderEntries_) {
		if (!entry.isPostDraw) {
			continue;
		}

		const ISceneObject* obj = GetRenderableObject(entry);
		if (obj && obj->IsActive() && IsVisible(*obj, cameraFrustum)) {
			Engine::SetPipeline(PSOType::Object3d, entry.renderingType);
			obj->Draw();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::EditObject(const ImVec2& windowSize, const ImVec2& imagePos) {
	for (const RenderEntry& entry : renderEntries_) {
		ISceneObject* obj = GetRenderableObject(entry);
		if (obj && obj->IsActive()) {
			obj->Manipulate(windowSize, imagePos);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::CreateObject(SceneLoader::LevelData* loadData) {
	if (!loadData) {
		return;
	}

	for (const SceneLoader::Objects& data : loadData->objects) {
		CreateObjectRecursive(data, ObjectHandle{});
	}
}

ObjectHandle SceneRenderer::CreateObjectRecursive(const SceneLoader::Objects& data, const ObjectHandle& parent) {
	const std::string renderingName = SelectRenderingName(data.material.shader);

	BaseGameObject* object = AddObject<BaseGameObject>(data.name, renderingName);
	if (!object) {
		return {};
	}

	const ObjectHandle handle = object->GetHandle();
	if (parent.IsValid()) {
		sceneWorld_.SetParent(handle, parent);
	}

	if (!data.modelName.empty()) {
		object->SetObject(data.modelName, SelectMaterialType(data.material.shader));
	}

	object->GetTransform()->SetSRT(data.srt);

	// colliderが設定されていたら
	if (!data.collidersData.empty()) {
		for (const SceneLoader::ColliderData& colliderData : data.collidersData) {
			if (colliderData.colliderType == "BOX") {
				object->SetCollider(colliderData.colliderTag, ColliderShape::AABB);
			} else if (colliderData.colliderType == "SPHERE") {
				object->SetCollider(colliderData.colliderTag, ColliderShape::Sphere);
			}

			BaseCollider* collider = object->GetCollider(colliderData.colliderTag);
			if (!collider) {
				continue;
			}

			collider->SetLocalPos(colliderData.center);
			if (colliderData.colliderType == "BOX") {
				BoxCollider* box = dynamic_cast<BoxCollider*>(collider);
				if (box) {
					box->SetSize(colliderData.size);
				}
			} else if (colliderData.colliderType == "SPHERE") {
				collider->SetRadius(colliderData.radius);
			}

			// colliderのtagが設定されていたら代入
			if (colliderData.colliderTag != "") {
				collider->SetCategory(colliderData.colliderTag);
			} else {
				collider->SetCategory("none");
			}

			// collisionFilterが設定されていたら
			if (!colliderData.filter.empty()) {
				for (size_t index = 0; index < colliderData.filter.size(); ++index) {
					collider->SetTarget(colliderData.filter[index]);
				}
			}
			collider->Update(data.srt);
		}
	}

	if (!data.modelName.empty() && data.material.shader == "PBR") {
		for (const auto& material : object->GetMaterials()) {
			AOENGINE::BaseMaterial* mat = material.second;
			AOENGINE::PBRMaterial* pbr = dynamic_cast<AOENGINE::PBRMaterial*>(mat);
			if (pbr) {
				pbr->SetParameter(data.material.roughness, data.material.metallic, data.material.iblStrength, data.material.normalMap);
			}
		}
	}

	object->SetIsRendering(data.isRendering_);

	for (const SceneLoader::Objects& childData : data.children) {
		CreateObjectRecursive(childData, handle);
	}

	return handle;
}

void SceneRenderer::ReleaseObject(ISceneObject* objPtr) {
	if (!objPtr) {
		return;
	}

	DestroyObject(objPtr->GetHandle());
}

std::vector<ObjectHandle> SceneRenderer::GetObjectHandles() const {
	return sceneWorld_.GetObjectHandles();
}

std::vector<ObjectHandle> SceneRenderer::GetRootObjectHandles() const {
	return sceneWorld_.GetRootObjectHandles();
}

SceneObject* SceneRenderer::FindObject(const ObjectHandle& handle) {
	return sceneWorld_.FindObject(handle);
}

const SceneObject* SceneRenderer::FindObject(const ObjectHandle& handle) const {
	return sceneWorld_.FindObject(handle);
}

bool SceneRenderer::SetParent(const ObjectHandle& child, const ObjectHandle& parent) {
	if (!sceneWorld_.SetParent(child, parent)) { return false; }
	SceneObject* childObject = sceneWorld_.FindObject(child);
	SceneObject* parentObject = sceneWorld_.FindObject(parent);
	if (BaseGameObject* childGameObject = dynamic_cast<BaseGameObject*>(childObject)) {
		if (BaseGameObject* parentGameObject = dynamic_cast<BaseGameObject*>(parentObject)) {
			childGameObject->GetTransform()->SetParent(parentGameObject->GetTransform()->GetWorldMatrix());
		} else {
			childGameObject->GetTransform()->ClearParent();
		}
	} else if (Sprite* childSprite = dynamic_cast<Sprite*>(childObject)) {
		if (Sprite* parentSprite = dynamic_cast<Sprite*>(parentObject)) {
			childSprite->GetTransform()->SetParent(parentSprite->GetTransform()->GetMatrix());
		} else {
			childSprite->GetTransform()->ClearParent();
		}
	}
	return true;
}

bool SceneRenderer::AddChild(const ObjectHandle& parent, const ObjectHandle& child) {
	return sceneWorld_.AddChild(parent, child);
}

bool SceneRenderer::RemoveChild(const ObjectHandle& parent, const ObjectHandle& child) {
	return sceneWorld_.RemoveChild(parent, child);
}

bool SceneRenderer::MoveToRoot(const ObjectHandle& handle) {
	if (!sceneWorld_.MoveToRoot(handle)) { return false; }
	if (BaseGameObject* object = dynamic_cast<BaseGameObject*>(sceneWorld_.FindObject(handle))) {
		object->GetTransform()->ClearParent();
	} else if (Sprite* sprite = dynamic_cast<Sprite*>(sceneWorld_.FindObject(handle))) {
		sprite->GetTransform()->ClearParent();
	}
	return true;
}

ObjectHandle SceneRenderer::PixelPick(const ImVec2& pixelPos, const ImVec2& imageSize) const {
	if (imageSize.x <= 0.0f || imageSize.y <= 0.0f ||
		pixelPos.x < 0.0f || pixelPos.y < 0.0f ||
		pixelPos.x >= imageSize.x || pixelPos.y >= imageSize.y) {
		return {};
	}

	// ImGui画像上のピクセルをDirectXのNDCへ変換し、Editor Cameraからレイを復元する。
	const float ndcX = pixelPos.x / imageSize.x * 2.0f - 1.0f;
	const float ndcY = 1.0f - pixelPos.y / imageSize.y * 2.0f;
	const Math::Matrix4x4 inverseViewProjection = AOENGINE::Render::GetViewProjectionMat().Inverse();
	const Math::Vector3 nearPoint = TransformCoord({ ndcX, ndcY, 0.0f }, inverseViewProjection);
	const Math::Vector3 farPoint = TransformCoord({ ndcX, ndcY, 1.0f }, inverseViewProjection);
	const Math::Vector3 rayDirection = (farPoint - nearPoint).Normalize();

	ObjectHandle closestHandle{};
	float closestDistance = FLT_MAX;
	for (const RenderEntry& entry : renderEntries_) {
		if (entry.isPostDraw) { continue; }

		const BaseGameObject* object = dynamic_cast<const BaseGameObject*>(GetRenderableObject(entry));
		if (!object || !object->IsActive() || !object->GetModel() || !object->GetTransform()) { continue; }

		const Math::Matrix4x4& world = object->GetTransform()->GetWorldMatrix();
		const Model* model = object->GetModel();
		for (uint32_t meshIndex = 0; meshIndex < model->GetMeshsNum(); ++meshIndex) {
			const Mesh* mesh = model->GetMesh(meshIndex);
			if (!mesh) { continue; }

			const auto& vertices = mesh->GetVertices();
			const auto& indices = mesh->GetIndices();
			for (uint32_t subMeshIndex = 0; subMeshIndex < mesh->GetSubMeshCount(); ++subMeshIndex) {
				const SubMesh& subMesh = mesh->GetSubMesh(subMeshIndex);
				if (subMesh.topology != D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) { continue; }

				const uint32_t endIndex = (std::min)(subMesh.firstIndex + subMesh.indexCount, static_cast<uint32_t>(indices.size()));
				for (uint32_t index = subMesh.firstIndex; index + 2 < endIndex; index += 3) {
					const int64_t i0 = static_cast<int64_t>(indices[index]) + subMesh.baseVertex;
					const int64_t i1 = static_cast<int64_t>(indices[index + 1]) + subMesh.baseVertex;
					const int64_t i2 = static_cast<int64_t>(indices[index + 2]) + subMesh.baseVertex;
					if (i0 < 0 || i1 < 0 || i2 < 0 ||
						i0 >= static_cast<int64_t>(vertices.size()) ||
						i1 >= static_cast<int64_t>(vertices.size()) ||
						i2 >= static_cast<int64_t>(vertices.size())) { continue; }

					const Math::Vector4& p0 = vertices[static_cast<size_t>(i0)].pos;
					const Math::Vector4& p1 = vertices[static_cast<size_t>(i1)].pos;
					const Math::Vector4& p2 = vertices[static_cast<size_t>(i2)].pos;
					const Math::Vector3 v0 = TransformCoord({ p0.x, p0.y, p0.z }, world);
					const Math::Vector3 v1 = TransformCoord({ p1.x, p1.y, p1.z }, world);
					const Math::Vector3 v2 = TransformCoord({ p2.x, p2.y, p2.z }, world);

					float distance = 0.0f;
					if (IntersectTriangle(nearPoint, rayDirection, v0, v1, v2, distance) && distance < closestDistance) {
						closestDistance = distance;
						closestHandle = entry.handle;
					}
				}
			}
		}
	}

	return closestHandle;
}

void SceneRenderer::DestroyObject(const ObjectHandle& handle) {
	sceneWorld_.DestroyObject(handle);
	RemoveInvalidRenderEntries();
}

BaseGameObject* SceneRenderer::DuplicateObject(BaseGameObject& source, const std::string& objectName) {
	const RenderEntry* sourceEntry = nullptr;
	for (const RenderEntry& entry : renderEntries_) {
		if (entry.handle == source.GetHandle()) {
			sourceEntry = &entry;
			break;
		}
	}

	if (!sourceEntry) {
		return nullptr;
	}

	BaseGameObject* duplicate = AddObject<BaseGameObject>(
		objectName, sourceEntry->renderingType, sourceEntry->renderQueue, sourceEntry->isPostDraw);
	if (!duplicate) {
		return nullptr;
	}

	if (source.GetModel()) {
		duplicate->SetObject(source.GetModel()->GetName());
	}
	if (source.GetTransform()) {
		duplicate->GetTransform()->SetSRT(source.GetTransform()->GetSRT());
	}
	duplicate->SetActive(source.IsSelfActive());
	duplicate->SetIsReflection(source.GetIsReflection());
	duplicate->SetIsRendering(source.GetIsRendering());
	duplicate->SetEnableShadow(source.GetEnableShadow());

	if (source.GetAnimator() && duplicate->GetModel()) {
		duplicate->SetAnimator();
	}

	const ObjectHandle parentHandle = sceneWorld_.GetParentHandle(source.GetHandle());
	if (parentHandle.IsValid()) {
		sceneWorld_.SetParent(duplicate->GetHandle(), parentHandle);
	}

	return duplicate;
}

void SceneRenderer::AddRenderEntry(const ObjectHandle& handle, const std::string& renderingName, int renderQueue, bool isPostDraw) {
	if (!sceneWorld_.IsValid(handle)) {
		return;
	}

	RemoveRenderEntry(handle);
	renderEntries_.push_back(RenderEntry{
		.handle = handle,
		.renderingType = renderingName,
		.renderQueue = renderQueue,
		.isPostDraw = isPostDraw
	});
}

void SceneRenderer::RemoveRenderEntry(const ObjectHandle& handle) {
	renderEntries_.erase(
		std::remove_if(
			renderEntries_.begin(),
			renderEntries_.end(),
			[handle](const RenderEntry& entry) {
				return entry.handle == handle;
			}),
		renderEntries_.end());
}

void SceneRenderer::RemoveInvalidRenderEntries() {
	renderEntries_.erase(
		std::remove_if(
			renderEntries_.begin(),
			renderEntries_.end(),
			[this](const RenderEntry& entry) {
				return !sceneWorld_.IsValid(entry.handle);
			}),
		renderEntries_.end());
}

ISceneObject* SceneRenderer::GetRenderableObject(const RenderEntry& entry) {
	return sceneWorld_.FindObjectAs<ISceneObject>(entry.handle);
}

const ISceneObject* SceneRenderer::GetRenderableObject(const RenderEntry& entry) const {
	return sceneWorld_.FindObjectAs<ISceneObject>(entry.handle);
}

bool SceneRenderer::IsVisible(const AOENGINE::ISceneObject& object, const Math::Frustum& frustum) const {
	if (!object.IsFrustumCullingEnabled()) {
		return true;
	}

	return frustum.Intersects(object.GetWorldBoundingSphere());
}

bool SceneRenderer::TryAddNormalInstancingBatch(
	const RenderEntry& entry,
	const AOENGINE::BaseGameObject& object,
	std::vector<AOENGINE::ModelInstancingRenderer::NormalBatch>& batches) const {
	if (entry.renderingType != "Object_Normal.json") {
		return false;
	}
	const bool isSkinned = object.CanUseSkinnedMaterialBatch();
	if (!isSkinned && !object.CanUseNormalInstancing()) { return false; }

	const AOENGINE::Model* model = object.GetModel();
	const AOENGINE::WorldTransform* transform = object.GetTransform();
	if (!model || !transform) {
		return false;
	}

	for (uint32_t index = 0; index < model->GetMeshsNum(); ++index) {
		AOENGINE::Mesh* mesh = model->GetMesh(index);
		if (!mesh) {
			return false;
		}

		AOENGINE::ModelInstancingRenderer::InstanceSource instance;
		instance.transform = transform;
		instance.materials.resize(model->GetMaterialSlotCount());
		for (uint32_t materialSlot = 0; materialSlot < model->GetMaterialSlotCount(); ++materialSlot) {
			const AOENGINE::BaseMaterial* slotMaterial = object.GetMaterial(materialSlot);
			const AOENGINE::Material* material = dynamic_cast<const AOENGINE::Material*>(slotMaterial);
			if (!material || material->GetShaderType() != MaterialShaderType::UniversalRender) {
				return false;
			}
			instance.materials[materialSlot] = AOENGINE::ModelInstancingRenderer::InstanceSource::MaterialSource{
				.material = &material->GetMaterialData(),
				.albedoTextureIndex = AOENGINE::TextureManager::GetInstance()->GetTextureDescriptorIndex(material->GetAlbedoTexture())
			};
		}

		for (uint32_t subMeshIndex = 0; subMeshIndex < mesh->GetSubMeshCount(); ++subMeshIndex) {
			const AOENGINE::SubMesh& subMesh = mesh->GetSubMesh(subMeshIndex);
			if (subMesh.topology != D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST || subMesh.materialSlot >= instance.materials.size()) {
				return false;
			}
		}

		if (isSkinned) {
			const AOENGINE::Animator* animator = object.GetAnimator();
			const AOENGINE::Skinning* skinning = animator ? animator->GetSkinning(index) : nullptr;
			if (!skinning) { return false; }

			AOENGINE::ModelInstancingRenderer::NormalBatch batch;
			batch.mesh = mesh;
			batch.vertexBufferView = skinning->GetVBV();
			batch.useVertexBufferOverride = true;
			batch.instances.push_back(std::move(instance));
			batches.push_back(std::move(batch));
			continue;
		}

		auto batchIt = std::find_if(
			batches.begin(),
			batches.end(),
			[mesh](const AOENGINE::ModelInstancingRenderer::NormalBatch& batch) {
				return batch.mesh == mesh && !batch.useVertexBufferOverride;
			});

		if (batchIt == batches.end()) {
			AOENGINE::ModelInstancingRenderer::NormalBatch batch;
			batch.mesh = mesh;
			batches.push_back(std::move(batch));
			batchIt = std::prev(batches.end());
		}

		batchIt->instances.push_back(std::move(instance));
	}

	return true;
}

void SceneRenderer::RegisterLightObjects() {
	LightGroup* lightGroup = AOENGINE::Render::GetLightGroup();
	if (!lightGroup) {
		return;
	}

	const ObjectHandle groupHandle = sceneWorld_.AddExternalObject(*lightGroup, lightGroup->GetName());

	if (DirectionalLight* light = lightGroup->GetDirectionalLight()) {
		const ObjectHandle lightHandle = sceneWorld_.AddExternalObject(*light, light->GetName());
		sceneWorld_.SetParent(lightHandle, groupHandle);
	}

	if (PointLight* light = lightGroup->GetPointLight()) {
		const ObjectHandle lightHandle = sceneWorld_.AddExternalObject(*light, light->GetName());
		sceneWorld_.SetParent(lightHandle, groupHandle);
	}

	if (SpotLight* light = lightGroup->GetSpotLight()) {
		const ObjectHandle lightHandle = sceneWorld_.AddExternalObject(*light, light->GetName());
		sceneWorld_.SetParent(lightHandle, groupHandle);
	}
}

void SceneRenderer::ChangeRenderingType(const std::string& renderingName, ISceneObject* gameObject) {
	if (!gameObject) {
		return;
	}

	for (RenderEntry& entry : renderEntries_) {
		if (GetRenderableObject(entry) == gameObject) {
			entry.renderingType = renderingName;
		}
	}
}

void SceneRenderer::SetRenderingQueue(const std::string& objName, int num) {
	for (RenderEntry& entry : renderEntries_) {
		ISceneObject* object = GetRenderableObject(entry);
		if (object && object->GetName() == objName) {
			entry.renderQueue = num;
		}
	}
}
