#pragma once
#include <memory>
#include <list>
#include "Engine/Module/Components/Collider/ICollider.h"

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
	static void AddCollider(ICollider* _collider);

	void RemoveCollider(ICollider* _collider);

	/// <summary>
	/// Colliderのリストを取得する
	/// </summary>
	/// <returns></returns>
	std::list<ICollider*>& GetColliderList() { return colliderList_; }

private:

	/// <summary>
	/// Colliderの追加
	/// </summary>
	/// <param name="_collider"></param>
	void AddColliderList(ICollider* _collider);

private:

	std::list<ICollider*> colliderList_;
 
};

