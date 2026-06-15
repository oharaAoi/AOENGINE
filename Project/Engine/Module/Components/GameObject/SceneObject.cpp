#include "SceneObject.h"

using namespace AOENGINE;

ObjectHandle SceneObject::GetHandle() const {
	return handle_;
}

const std::string& SceneObject::GetName() const {
	return name_;
}

void SceneObject::SetName(const std::string& name) {
	name_ = name;
}

bool SceneObject::IsActive() const {
	return isActive_;
}

void SceneObject::SetActive(bool active) {
	isActive_ = active;
}

void AOENGINE::SceneObject::AddChild(SceneObject* child) {
	if (child) {
		children_.push_back(child);
	}
}

void AOENGINE::SceneObject::DeleteChild(SceneObject* child) {
	children_.erase(
		std::remove(children_.begin(), children_.end(), child),
		children_.end()
	);
}
