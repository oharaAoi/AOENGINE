#include "SceneObject.h"
#include <algorithm>

using namespace AOENGINE;

namespace {
uint64_t gNextSceneObjectId = 1;
}

uint64_t SceneObject::GetSceneId() const {
	return sceneId_;
}

void SceneObject::SetSceneId(uint64_t sceneId) {
	sceneId_ = sceneId;
	if (sceneId >= gNextSceneObjectId) {
		gNextSceneObjectId = sceneId + 1;
	}
}

ObjectHandle SceneObject::GetHandle() const {
	return handle_;
}

void SceneObject::SetHandle(const ObjectHandle& handle) {
	handle_ = handle;
	if (handle.IsValid() && sceneId_ == 0) {
		sceneId_ = gNextSceneObjectId++;
	}
}

const std::string& SceneObject::GetName() const {
	return name_;
}

void SceneObject::SetName(const std::string& name) {
	name_ = name;
}

bool SceneObject::IsActive() const {
	return isActiveSelf_ && (parent_ == nullptr || parent_->IsActive());
}

bool SceneObject::IsSelfActive() const {
	return isActiveSelf_;
}

void SceneObject::SetActive(bool active) {
	isActiveSelf_ = active;
}

void SceneObject::SetPrefabSource(const std::string& prefabName) {
	prefabSource_ = prefabName;
}

void SceneObject::ClearPrefabSource() {
	prefabSource_.clear();
}

void AOENGINE::SceneObject::AddChild(SceneObject* child) {
	if (!child || child == this) { return; }
	if (std::find(children_.begin(), children_.end(), child) == children_.end()) {
		children_.push_back(child);
	}
	child->parent_ = this;
}

void AOENGINE::SceneObject::DeleteChild(SceneObject* child) {
	if (child && child->parent_ == this) {
		child->parent_ = nullptr;
	}
	children_.erase(
		std::remove(children_.begin(), children_.end(), child),
		children_.end()
	);
}

void AOENGINE::SceneObject::ClearChildren() {
	for (SceneObject* child : children_) {
		if (child && child->parent_ == this) {
			child->parent_ = nullptr;
		}
	}
	children_.clear();
}
