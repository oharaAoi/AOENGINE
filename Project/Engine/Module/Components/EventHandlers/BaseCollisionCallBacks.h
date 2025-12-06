#pragma once
//  c++
#include <functional>
// collider
#include "Engine/System/Manager/CollisionManager.h"
#include "Engine/Module/Components/Collider/BaseCollider.h"

#define __CALLBACK_PLACEHOLDERS_12 std::placeholders::_1, std::placeholders::_2

namespace AOENGINE {

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

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	virtual void CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const, [[maybe_unused]] AOENGINE::BaseCollider* const) = 0;
	virtual void CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const, [[maybe_unused]] AOENGINE::BaseCollider* const) = 0;
	virtual void CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const, [[maybe_unused]] AOENGINE::BaseCollider* const) = 0;

	/// <summary>
	/// コールバック関数を設定する
	/// </summary>
	void SetCallBacks();

	void SetPair(CollisionManager* collisionManager, const std::string& attacker, const std::string& diffence);

protected:

	CollisionManager::CallBackKinds callBacks_;

};

}