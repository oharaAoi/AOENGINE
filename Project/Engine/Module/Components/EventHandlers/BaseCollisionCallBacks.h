#pragma once
//  c++
#include <functional>
// collider
#include "Engine/System/Manager/CollisionManager.h"
#include "Engine/Module/Components/Collider/ICollider.h"

#define __CALLBACK_PLACEHOLDERS_12 std::placeholders::_1, std::placeholders::_2

/// <summary>
/// CallBack関数を設定するクラス
/// </summary>
class BaseCollisionCallBacks {
public:

	BaseCollisionCallBacks() {};
	virtual ~BaseCollisionCallBacks() = default;

	/// <summary>
	/// この処理はセッタで色々設定した後に実行
	/// </summary>
	virtual void Init() = 0;

	virtual void Update() = 0;

	virtual void CollisionEnter([[maybe_unused]] ICollider* const, [[maybe_unused]] ICollider* const) = 0;
	virtual void CollisionStay([[maybe_unused]] ICollider* const, [[maybe_unused]] ICollider* const) = 0;
	virtual void CollisionExit([[maybe_unused]] ICollider* const, [[maybe_unused]] ICollider* const) = 0;

	void SetCallBacks();

	void SetPair(CollisionManager* collisionManager, const std::string& attacker, const std::string& diffence);

protected:

	CollisionManager::CallBackKinds callBacks_;

};

