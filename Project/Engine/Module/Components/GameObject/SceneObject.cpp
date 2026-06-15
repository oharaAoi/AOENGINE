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
