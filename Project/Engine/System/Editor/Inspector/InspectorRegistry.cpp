#include "InspectorRegistry.h"
#include <algorithm>

AOENGINE::InspectorRegistry& AOENGINE::InspectorRegistry::GetInstance() {
	static InspectorRegistry instance;
	return instance;
}

bool AOENGINE::InspectorRegistry::DrawObject(SceneObject& object) {
	if (objectDrawers_.empty()) {
		return false;
	}

	const std::type_index exactType = std::type_index(typeid(object));
	auto exact = objectDrawers_.find(exactType);
	if (exact != objectDrawers_.end() && exact->second(object)) {
		return true;
	}

	for (auto it = objectDrawerOrder_.rbegin(); it != objectDrawerOrder_.rend(); ++it) {
		if (*it == exactType) {
			continue;
		}

		auto fallback = objectDrawers_.find(*it);
		if (fallback != objectDrawers_.end() && fallback->second(object)) {
			return true;
		}
	}

	return false;
}

bool AOENGINE::InspectorRegistry::DrawComponent(::IComponent& component) {
	if (componentDrawers_.empty()) {
		return false;
	}

	const std::type_index exactType = std::type_index(typeid(component));
	auto exact = componentDrawers_.find(exactType);
	if (exact != componentDrawers_.end() && exact->second(component)) {
		return true;
	}

	for (auto it = componentDrawerOrder_.rbegin(); it != componentDrawerOrder_.rend(); ++it) {
		if (*it == exactType) {
			continue;
		}

		auto fallback = componentDrawers_.find(*it);
		if (fallback != componentDrawers_.end() && fallback->second(component)) {
			return true;
		}
	}

	return false;
}

bool AOENGINE::InspectorRegistry::HasObjectDrawer(const std::type_index& type) const {
	return objectDrawers_.find(type) != objectDrawers_.end();
}

bool AOENGINE::InspectorRegistry::HasComponentDrawer(const std::type_index& type) const {
	return componentDrawers_.find(type) != componentDrawers_.end();
}

void AOENGINE::InspectorRegistry::UnregisterObjectDrawer(const std::type_index& type) {
	objectDrawers_.erase(type);
	objectDrawerOrder_.erase(
		std::remove(objectDrawerOrder_.begin(), objectDrawerOrder_.end(), type),
		objectDrawerOrder_.end()
	);
}

void AOENGINE::InspectorRegistry::UnregisterComponentDrawer(const std::type_index& type) {
	componentDrawers_.erase(type);
	componentDrawerOrder_.erase(
		std::remove(componentDrawerOrder_.begin(), componentDrawerOrder_.end(), type),
		componentDrawerOrder_.end()
	);
}

void AOENGINE::InspectorRegistry::ClearObjectDrawers() {
	objectDrawers_.clear();
	objectDrawerOrder_.clear();
}

void AOENGINE::InspectorRegistry::ClearComponentDrawers() {
	componentDrawers_.clear();
	componentDrawerOrder_.clear();
}

void AOENGINE::InspectorRegistry::Clear() {
	ClearObjectDrawers();
	ClearComponentDrawers();
}
