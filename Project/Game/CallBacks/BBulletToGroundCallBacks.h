#pragma once
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"
#include "Engine/System/Manager/CollisionManager.h"
// Game
#include "Game/Actor/Boss/Bullet/BossBulletManager.h"
#include "Game/WorldObject/Floor.h"

/// <summary>
/// Bossの弾と地面のあたり判定
/// </summary>
class BBulletToGroundCallBacks :
	public BaseCollisionCallBacks {
public:

	BBulletToGroundCallBacks(CollisionManager* _collisionManager) : pCollisionManager_(_collisionManager) {};
	~BBulletToGroundCallBacks() = default;

	void Init() override;

	void Update() override;

	void CollisionEnter([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const ground) override;
	void CollisionStay([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const ground)  override;
	void CollisionExit([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const ground)  override;

public:

	void SetBossBulletManager(BossBulletManager* _manager) { pBossBulletManager_ = _manager; }

	void SetGround(Floor* _floor) { pFloor_ = _floor; }

private:

	CollisionManager* pCollisionManager_;

	BossBulletManager* pBossBulletManager_ = nullptr;
	Floor* pFloor_ = nullptr;


};

