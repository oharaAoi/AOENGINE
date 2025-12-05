#pragma once
#include <memory>
#include <list>
#include "Engine/Module/Components/Collider/BaseCollider.h"

/// <summary>
/// Colliderを収集する
/// </summary>
class ColliderCollector {
public:

	ColliderCollector() = default; // コンストラクタをprivateに（シングルトン）
	ColliderCollector(const ColliderCollector&) = delete;
	ColliderCollector& operator=(const ColliderCollector&) = delete;

	static ColliderCollector* GetInstance();

	void Init();

	void Update();

	void Draw() const;

public:

	void Reset() { colliderList_.clear(); }

	/// <summary>
	/// Colliderの追加
	/// </summary>
	/// <param name="_collider"></param>
	static void AddCollider(BaseCollider* _collider);

	/// <summary>
	/// Colldierを削除する
	/// </summary>
	/// <param name="_collider"></param>
	void RemoveCollider(BaseCollider* _collider);

	/// <summary>
	/// Colliderのリストを取得する
	/// </summary>
	/// <returns></returns>
	std::list<BaseCollider*>& GetColliderList() { return colliderList_; }

private:

	/// <summary>
	/// Colliderの追加
	/// </summary>
	/// <param name="_collider"></param>
	void AddColliderList(BaseCollider* _collider);

private:

	std::list<BaseCollider*> colliderList_;
 
};

