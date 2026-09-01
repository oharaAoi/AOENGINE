#include "SceneRenderer.h"

#include <algorithm>
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

}

SceneRenderer* AOENGINE::SceneRenderer::GetInstance() {
	static SceneRenderer instance;
	return &instance;
}

void SceneRenderer::Finalize() {
	renderEntries_.clear();
	sceneWorld_.Clear();
	modelInstancingRenderer_.Finalize();

	if (particleManager_) {
		particleManager_->Finalize();
	}
	if (gpuParticleManager_) {
		gpuParticleManager_->Finalize();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::Init() {
	renderEntries_.clear();
	sceneWorld_.Clear();

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
	const Math::Frustum cameraFrustum = Math::Frustum::FromViewProjection(AOENGINE::Render::GetViewProjectionMat());

	// 通常3Dモデルは可能な限りInstancing batchへ集約し、最後にまとめて描画します。
	std::vector<AOENGINE::ModelInstancingRenderer::NormalBatch> normalInstancingBatches;
	modelInstancingRenderer_.BeginFrame();

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
	duplicate->SetActive(source.IsActive());
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
