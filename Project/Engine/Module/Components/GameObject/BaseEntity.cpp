#include "BaseEntity.h"

#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Render/SceneRenderer.h"

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
