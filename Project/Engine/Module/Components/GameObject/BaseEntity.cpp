#include "BaseEntity.h"

#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/Utilities/Logger.h"

using namespace AOENGINE;

BaseEntity::BaseEntity(BaseGameObject* object) {
	Bind(object);
}

void BaseEntity::Bind(BaseGameObject* object) {
	objectHandle_ = object ? object->GetHandle() : ObjectHandle{};
}

void BaseEntity::Bind(const ObjectHandle& handle) {
	objectHandle_ = handle;
	if (!IsValid()) {
		objectHandle_ = {};
	}
}

void BaseEntity::Unbind() {
	objectHandle_ = {};
}

BaseGameObject* AOENGINE::BaseEntity::InstantiatePrefab(const std::string& prefabName) {
	AOENGINE::SceneObject* root =
		AOENGINE::PrefabManager::GetInstance()->Instantiate("Enemy");

	AOENGINE::BaseGameObject* obj =
		dynamic_cast<AOENGINE::BaseGameObject*>(root);
	if (obj == nullptr) {
		AOENGINE::Logger::AssertLog(prefabName + ": prefabがnullでした");
	}
	return obj;
}

bool BaseEntity::IsValid() const {
	return GetGameObject() != nullptr;
}

BaseGameObject* BaseEntity::GetGameObject() const {
	if (!objectHandle_.IsValid()) {
		return nullptr;
	}

	SceneObject* object = SceneRenderer::GetInstance()->FindObject(objectHandle_);
	return dynamic_cast<BaseGameObject*>(object);
}

WorldTransform* BaseEntity::GetTransform() const {
	BaseGameObject* object = GetGameObject();
	return object ? object->GetTransform() : nullptr;
}

BaseCollider* BaseEntity::GetCollider(const std::string& tag) const {
	BaseGameObject* object = GetGameObject();
	return object ? object->GetCollider(tag) : nullptr;
}

void BaseEntity::SetIsActive(bool isActive) {
	if (BaseGameObject* object = GetGameObject()) {
		object->SetActive(isActive);
	}
}

bool BaseEntity::GetIsActive() const {
	const BaseGameObject* object = GetGameObject();
	return object ? object->IsActive() : false;
}

void BaseEntity::SetRendering(bool isRendering) {
	if (BaseGameObject* object = GetGameObject()) {
		object->SetIsRendering(isRendering);
	}
}

Math::Vector3 BaseEntity::GetPosition() const {
	const BaseGameObject* object = GetGameObject();
	return object ? object->GetPosition() : Math::Vector3{};
}

void BaseEntity::Destroy() {
	if (!objectHandle_.IsValid()) {
		return;
	}

	SceneRenderer::GetInstance()->DestroyObject(objectHandle_);
	Unbind();
}

bool BaseEntity::SetParent(const BaseEntity* parent) {
	if (!IsValid() || !parent || !parent->IsValid()) {
		return false;
	}

	return SceneRenderer::GetInstance()->SetParent(objectHandle_, parent->objectHandle_);
}

bool BaseEntity::MoveToRoot() {
	if (!IsValid()) {
		return false;
	}

	return SceneRenderer::GetInstance()->MoveToRoot(objectHandle_);
}
