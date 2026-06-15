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

/// <summary>
/// SceneObjectやIComponentの型に対応するInspector描画関数を管理するRegistry。
/// Runtime側のクラスにDebug_Guiを持たせず、Editor側で型ごとの描画先を解決する。
/// </summary>
class InspectorRegistry final {
public:
	/// <summary>
	/// SceneObject用のInspector描画関数。
	/// trueを返した場合は描画済みとして扱う。
	/// </summary>
	using ObjectDrawer = std::function<bool(SceneObject&)>;

	/// <summary>
	/// IComponent用のInspector描画関数。
	/// trueを返した場合は描画済みとして扱う。
	/// </summary>
	using ComponentDrawer = std::function<bool(::IComponent&)>;

	/// <summary>
	/// InspectorRegistryのSingletonインスタンスを取得する。
	/// </summary>
	static InspectorRegistry& GetInstance();
	InspectorRegistry(const InspectorRegistry&) = delete;
	InspectorRegistry& operator=(const InspectorRegistry&) = delete;
	InspectorRegistry(InspectorRegistry&&) = delete;
	InspectorRegistry& operator=(InspectorRegistry&&) = delete;

	/// <summary>
	/// SceneObject派生型に対応するInspector描画関数を登録する。
	/// </summary>
	/// <typeparam name="T">Inspector対象のSceneObject派生型。</typeparam>
	/// <param name="drawer">型変換後に呼び出す描画関数。</param>
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

	/// <summary>
	/// IComponent派生型に対応するInspector描画関数を登録する。
	/// </summary>
	/// <typeparam name="T">Inspector対象のIComponent派生型。</typeparam>
	/// <param name="drawer">型変換後に呼び出す描画関数。</param>
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

	/// <summary>
	/// 指定したSceneObject派生型のInspector登録を解除する。
	/// </summary>
	template<class T>
	void UnregisterObjectDrawer() {
		UnregisterObjectDrawer(std::type_index(typeid(T)));
	}

	/// <summary>
	/// 指定したIComponent派生型のInspector登録を解除する。
	/// </summary>
	template<class T>
	void UnregisterComponentDrawer() {
		UnregisterComponentDrawer(std::type_index(typeid(T)));
	}

	/// <summary>
	/// 指定したSceneObject派生型のInspectorが登録済みか調べる。
	/// </summary>
	template<class T>
	bool HasObjectDrawer() const {
		return HasObjectDrawer(std::type_index(typeid(T)));
	}

	/// <summary>
	/// 指定したIComponent派生型のInspectorが登録済みか調べる。
	/// </summary>
	template<class T>
	bool HasComponentDrawer() const {
		return HasComponentDrawer(std::type_index(typeid(T)));
	}

	/// <summary>
	/// objectの実型に対応するInspectorを探して描画する。
	/// </summary>
	/// <returns>描画できるInspectorが見つかった場合はtrue。</returns>
	bool DrawObject(SceneObject& object);

	/// <summary>
	/// componentの実型に対応するInspectorを探して描画する。
	/// </summary>
	/// <returns>描画できるInspectorが見つかった場合はtrue。</returns>
	bool DrawComponent(::IComponent& component);

	/// <summary>
	/// typeに対応するSceneObject Inspectorが登録済みか調べる。
	/// </summary>
	bool HasObjectDrawer(const std::type_index& type) const;

	/// <summary>
	/// typeに対応するComponent Inspectorが登録済みか調べる。
	/// </summary>
	bool HasComponentDrawer(const std::type_index& type) const;

	/// <summary>
	/// typeに対応するSceneObject Inspectorの登録を解除する。
	/// </summary>
	void UnregisterObjectDrawer(const std::type_index& type);

	/// <summary>
	/// typeに対応するComponent Inspectorの登録を解除する。
	/// </summary>
	void UnregisterComponentDrawer(const std::type_index& type);

	/// <summary>
	/// SceneObject Inspectorの登録をすべて削除する。
	/// </summary>
	void ClearObjectDrawers();

	/// <summary>
	/// Component Inspectorの登録をすべて削除する。
	/// </summary>
	void ClearComponentDrawers();

	/// <summary>
	/// すべてのInspector登録を削除する。
	/// </summary>
	void Clear();

private:
	InspectorRegistry() = default;

	std::unordered_map<std::type_index, ObjectDrawer> objectDrawers_;
	std::unordered_map<std::type_index, ComponentDrawer> componentDrawers_;
	std::vector<std::type_index> objectDrawerOrder_;
	std::vector<std::type_index> componentDrawerOrder_;
};

}
