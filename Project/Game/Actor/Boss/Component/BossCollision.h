#pragma once
#include <functional>
#include <string>
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"

namespace AOENGINE {
	class BaseGameObject;
	class BaseCollider;
	class CollisionManager;
}

/// <summary>
/// ボスの当たり判定を担当するコンポーネント
/// </summary>
class BossCollision : public AOENGINE::BaseCollisionCallBacks {
public:

	BossCollision() = default;
	~BossCollision() override = default;

	// コライダー作成、登録
	void CreateCollider(AOENGINE::BaseGameObject* body, const Math::Vector3& halfSize);
	void RegisterPair(AOENGINE::CollisionManager* collisionManager, const std::string& attackerCategory);

	///Enterのhitの時に呼ぶコールバック
	void SetOnHit(std::function<void()> onHit) { onHit_ = std::move(onHit); }

	// BaseCollisionCallBacks
	void Init() override {}
	void Update() override {}
	void CollisionEnter(AOENGINE::BaseCollider* const attacker, AOENGINE::BaseCollider* const boss) override;
	void CollisionStay(AOENGINE::BaseCollider* const attacker, AOENGINE::BaseCollider* const boss) override;
	void CollisionExit(AOENGINE::BaseCollider* const attacker, AOENGINE::BaseCollider* const boss) override;

	// ボスColliderのカテゴリ名
	const std::string kCategory = "Boss";

private:

	AOENGINE::BaseCollider* collider_ = nullptr;
	std::function<void()> onHit_;
public:
	AOENGINE::BaseCollider* GetCollider() const { return collider_; }
};
