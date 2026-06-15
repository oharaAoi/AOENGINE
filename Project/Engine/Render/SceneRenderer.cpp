#include "SceneRenderer.h"

#include <algorithm>
#include <format>

#include "Engine/Core/Engine.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"
#include "Engine/Module/Components/Materials/PBRMaterial.h"
#include "Engine/Render/Render.h"
#include "Engine/System/Editor/Window/EditorWindows.h"

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
	particleManager_->Update();
	gpuParticleManager_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::Draw() const {
	// 影の描画
	AOENGINE::Render::SetShadowMap();
	for (const RenderEntry& entry : renderEntries_) {
		const ISceneObject* obj = GetRenderableObject(entry);
		if (obj && obj->IsActive()) {
			obj->PreDraw();
		}
	}

	// objectの描画
	std::vector<RenderTargetType> types;
	types.push_back(RenderTargetType::Object3D_RenderTarget);
	types.push_back(RenderTargetType::MotionVector_RenderTarget);
	AOENGINE::Render::SetRenderTarget(types, AOENGINE::GraphicsContext::GetInstance()->GetDxCommon()->GetDepthHandle());

	AOENGINE::Render::ChangeShadowMap();
	for (const RenderEntry& entry : renderEntries_) {
		if (entry.isPostDraw) {
			continue;
		}

		const ISceneObject* obj = GetRenderableObject(entry);
		if (obj && obj->IsActive()) {
			Engine::SetPipeline(PSOType::Object3d, entry.renderingType);
			obj->Draw();
		}
	}

	// particleの描画
	particleManager_->Draw();
	gpuParticleManager_->Draw();
}

void SceneRenderer::PostDraw() const {
	for (const RenderEntry& entry : renderEntries_) {
		if (!entry.isPostDraw) {
			continue;
		}

		const ISceneObject* obj = GetRenderableObject(entry);
		if (obj && obj->IsActive()) {
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

void AOENGINE::SceneRenderer::Debug_Gui() {
	for (const RenderEntry& entry : renderEntries_) {
		ISceneObject* obj = GetRenderableObject(entry);
		if (!obj) {
			continue;
		}

		std::string addrStr = std::format("{}", static_cast<const void*>(obj));
		std::string name = obj->GetName() + "##" + addrStr;
		if (ImGui::TreeNode(name.c_str())) {
			//obj->Debug_Gui();
			ImGui::TreePop();
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
			AOENGINE::BaseMaterial* mat = material.second.get();
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
	return sceneWorld_.SetParent(child, parent);
}

bool SceneRenderer::AddChild(const ObjectHandle& parent, const ObjectHandle& child) {
	return sceneWorld_.AddChild(parent, child);
}

bool SceneRenderer::RemoveChild(const ObjectHandle& parent, const ObjectHandle& child) {
	return sceneWorld_.RemoveChild(parent, child);
}

bool SceneRenderer::MoveToRoot(const ObjectHandle& handle) {
	return sceneWorld_.MoveToRoot(handle);
}

void SceneRenderer::DestroyObject(const ObjectHandle& handle) {
	sceneWorld_.DestroyObject(handle);
	RemoveInvalidRenderEntries();
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
