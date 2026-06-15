#pragma once

#include "Engine/Module/Components/GameObject/ISceneObject.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace AOENGINE {

/// <summary>
/// SceneObjectの所有、ObjectHandleの発行、Hierarchy用の親子関係を管理するシーン単位のコンテナ。
/// </summary>
class SceneWorld {
public:
	SceneWorld() = default;
	~SceneWorld() = default;

	SceneWorld(const SceneWorld&) = delete;
	SceneWorld& operator=(const SceneWorld&) = delete;
	SceneWorld(SceneWorld&&) = default;
	SceneWorld& operator=(SceneWorld&&) = default;

	/// <summary>
	/// 登録されている全オブジェクトを破棄し、既存のObjectHandleを無効化します。
	/// </summary>
	void Clear();

	/// <summary>
	/// SceneObjectをSceneWorldに登録し、生成済みObjectHandleを返します。
	/// </summary>
	ObjectHandle AddObject(std::unique_ptr<SceneObject> object, const std::string& name = {});

	/// <summary>
	/// SceneObject派生型を生成してSceneWorldに登録します。
	/// </summary>
	template<class T, class... Args>
	T* CreateObject(const std::string& name, Args&&... args) {
		static_assert(std::is_base_of_v<SceneObject, T>, "T must derive from SceneObject.");

		// AddObjectで所有権をSceneWorldへ移すため、登録前に返却用ポインタを保持しておく。
		auto object = std::make_unique<T>(std::forward<Args>(args)...);
		T* result = object.get();

		ObjectHandle handle = AddObject(std::move(object), name);
		if (!handle.IsValid()) {
			return nullptr;
		}

		// 実際にシーンで更新されるオブジェクトだけ初期化処理を呼び出す。
		if constexpr (std::is_base_of_v<ISceneObject, T>) {
			result->Init();
		}

		return result;
	}

	/// <summary>
	/// 指定したObjectHandleのオブジェクトと子階層を破棄します。
	/// </summary>
	void DestroyObject(ObjectHandle handle);

	/// <summary>
	/// ObjectHandleが現在のSceneWorldで有効か判定します。
	/// </summary>
	bool IsValid(ObjectHandle handle) const;

	/// <summary>
	/// ObjectHandleからSceneObjectを取得します。無効な場合はnullptrを返します。
	/// </summary>
	SceneObject* FindObject(ObjectHandle handle);
	const SceneObject* FindObject(ObjectHandle handle) const;

	/// <summary>
	/// ObjectHandleから指定型のSceneObject派生クラスを取得します。
	/// </summary>
	template<class T>
	T* FindObjectAs(ObjectHandle handle) {
		return dynamic_cast<T*>(FindObject(handle));
	}

	/// <summary>
	/// ObjectHandleから指定型のSceneObject派生クラスを取得します。
	/// </summary>
	template<class T>
	const T* FindObjectAs(ObjectHandle handle) const {
		return dynamic_cast<const T*>(FindObject(handle));
	}

	/// <summary>
	/// SceneWorldが所有する全オブジェクトのObjectHandleを返します。
	/// </summary>
	std::vector<ObjectHandle> GetObjectHandles() const;

	/// <summary>
	/// 親を持たないルートオブジェクトのObjectHandleを返します。
	/// </summary>
	std::vector<ObjectHandle> GetRootObjectHandles() const;

	/// <summary>
	/// 指定オブジェクトの親ObjectHandleを返します。親がない場合は無効なObjectHandleを返します。
	/// </summary>
	ObjectHandle GetParentHandle(ObjectHandle child) const;

	/// <summary>
	/// childをparentの子にします。parentが無効な場合はchildをルートに戻します。
	/// </summary>
	bool SetParent(ObjectHandle child, ObjectHandle parent);

	/// <summary>
	/// parentにchildを追加します。
	/// </summary>
	bool AddChild(ObjectHandle parent, ObjectHandle child);

	/// <summary>
	/// parentからchildを外し、childをルートオブジェクトに戻します。
	/// </summary>
	bool RemoveChild(ObjectHandle parent, ObjectHandle child);

	/// <summary>
	/// 指定オブジェクトを親から外し、ルートオブジェクトにします。
	/// </summary>
	bool MoveToRoot(ObjectHandle handle);

	/// <summary>
	/// ISceneObject派生オブジェクトのUpdateを呼び出します。
	/// </summary>
	void Update();

	/// <summary>
	/// ISceneObject派生オブジェクトのPostUpdateを呼び出します。
	/// </summary>
	void PostUpdate();

	/// <summary>
	/// 指定オブジェクトがルートに登録されているか判定します。
	/// </summary>
	bool IsRootObject(ObjectHandle handle) const;

private:
	/// <summary>
	/// ObjectHandleのindexに対応する実体スロット。
	/// generationを進めることで、破棄済みオブジェクトを指す古いObjectHandleを無効化します。
	/// </summary>
	struct ObjectSlot {
		std::unique_ptr<SceneObject> object;
		uint32_t generation = 1;
		bool isAlive = false;
	};

	/// <summary>
	/// 新規オブジェクト用に未使用スロット、または再利用可能スロットのObjectHandleを発行します。
	/// </summary>
	ObjectHandle AllocateHandle();

	/// <summary>
	/// ObjectHandleに対応するスロットを解放し、次回以降のハンドル検証で失敗するようgenerationを進めます。
	/// </summary>
	void ReleaseHandle(ObjectHandle handle);

	/// <summary>
	/// 指定オブジェクトを、現在親として参照している全オブジェクトの子リストから取り除きます。
	/// </summary>
	void DetachFromParents(SceneObject* object);

	/// <summary>
	/// ルート管理リストから指定indexのObjectHandleを取り除きます。
	/// </summary>
	void RemoveRootObject(ObjectHandle handle);

	/// <summary>
	/// 指定ObjectHandleをルート管理リストに追加します。既に登録済みの場合は何もしません。
	/// </summary>
	bool AddRootObject(ObjectHandle handle);

	/// <summary>
	/// ObjectHandleのgenerationを次の値へ進めます。0は無効値のため使用しません。
	/// </summary>
	static uint32_t NextGeneration(uint32_t generation);

	/// <summary>
	/// root以下の子階層にtargetが含まれているか再帰的に判定します。
	/// </summary>
	static bool ContainsObjectRecursive(const SceneObject* root, const SceneObject* target);

private:
	// ObjectHandle.indexからSceneObject実体へアクセスするためのスロット配列。
	std::vector<ObjectSlot> objectSlots_;

	// 破棄済みスロットのindexを保持し、次回のAddObjectで再利用する。
	std::vector<uint32_t> reusableObjectIndices_;

	// Hierarchy表示や走査の起点となる、親を持たないオブジェクトのハンドル一覧。
	std::vector<ObjectHandle> rootObjectHandles_;
};

}
