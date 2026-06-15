#pragma once
#include <functional>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>
#include "Engine/Module/Components/GameObject/SceneObject.h"
#include "Engine/Module/Components/IComponent.h"

namespace AOENGINE {

class InspectorRegistry final {
public:
	using ObjectDrawer = std::function<bool(SceneObject&)>;
	using ComponentDrawer = std::function<bool(::IComponent&)>;

	static InspectorRegistry& GetInstance();
	InspectorRegistry(const InspectorRegistry&) = delete;
	InspectorRegistry& operator=(const InspectorRegistry&) = delete;
	InspectorRegistry(InspectorRegistry&&) = delete;
	InspectorRegistry& operator=(InspectorRegistry&&) = delete;

	template<class T>
	void RegisterObjectDrawer(std::function<void(T&)> drawer) {
		static_assert(std::is_base_of_v<SceneObject, T>, "T must derive from SceneObject.");

		const std::type_index type = std::type_index(typeid(T));
		auto result = objectDrawers_.insert_or_assign(
			type,
			[drawer = std::move(drawer)](SceneObject& object) -> bool {
				if (!drawer) {
					return false;
				}

				T* typedObject = dynamic_cast<T*>(&object);
				if (!typedObject) {
					return false;
				}

				drawer(*typedObject);
				return true;
			}
		);

		if (result.second) {
			objectDrawerOrder_.push_back(type);
		}
	}

	template<class T>
	void RegisterComponentDrawer(std::function<void(T&)> drawer) {
		static_assert(std::is_base_of_v<::IComponent, T>, "T must derive from IComponent.");

		const std::type_index type = std::type_index(typeid(T));
		auto result = componentDrawers_.insert_or_assign(
			type,
			[drawer = std::move(drawer)](::IComponent& component) -> bool {
				if (!drawer) {
					return false;
				}

				T* typedComponent = dynamic_cast<T*>(&component);
				if (!typedComponent) {
					return false;
				}

				drawer(*typedComponent);
				return true;
			}
		);

		if (result.second) {
			componentDrawerOrder_.push_back(type);
		}
	}

	template<class T>
	void UnregisterObjectDrawer() {
		UnregisterObjectDrawer(std::type_index(typeid(T)));
	}

	template<class T>
	void UnregisterComponentDrawer() {
		UnregisterComponentDrawer(std::type_index(typeid(T)));
	}

	template<class T>
	bool HasObjectDrawer() const {
		return HasObjectDrawer(std::type_index(typeid(T)));
	}

	template<class T>
	bool HasComponentDrawer() const {
		return HasComponentDrawer(std::type_index(typeid(T)));
	}

	bool DrawObject(SceneObject& object);
	bool DrawComponent(::IComponent& component);

	bool HasObjectDrawer(const std::type_index& type) const;
	bool HasComponentDrawer(const std::type_index& type) const;

	void UnregisterObjectDrawer(const std::type_index& type);
	void UnregisterComponentDrawer(const std::type_index& type);

	void ClearObjectDrawers();
	void ClearComponentDrawers();
	void Clear();

private:
	InspectorRegistry() = default;

	std::unordered_map<std::type_index, ObjectDrawer> objectDrawers_;
	std::unordered_map<std::type_index, ComponentDrawer> componentDrawers_;
	std::vector<std::type_index> objectDrawerOrder_;
	std::vector<std::type_index> componentDrawerOrder_;
};

}
