#pragma once
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"
#include "Engine/System/Manager/CollisionManager.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
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

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;

	// 衝突開始
	void CollisionEnter([[maybe_unused]] BaseCollider* const bullet, [[maybe_unused]] BaseCollider* const ground) override;
	// 衝突中
	void CollisionStay([[maybe_unused]] BaseCollider* const bullet, [[maybe_unused]] BaseCollider* const ground)  override;
	// 衝突終わり
	void CollisionExit([[maybe_unused]] BaseCollider* const bullet, [[maybe_unused]] BaseCollider* const ground)  override;

public:

	void SetBossBulletManager(BossBulletManager* _manager) { pBossBulletManager_ = _manager; }

	void SetGround(Floor* _floor) { pFloor_ = _floor; }

private:

	CollisionManager* pCollisionManager_;

	BossBulletManager* pBossBulletManager_ = nullptr;
	Floor* pFloor_ = nullptr;

	BaseParticles* hitEffect_;

};

