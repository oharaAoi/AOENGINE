#pragma once
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"
#include "Engine/System/Manager/CollisionManager.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
// Game
#include "Game/Actor/Player/Bullet/PlayerBulletManager.h"
#include "Game/Actor/Boss/Boss.h"

/// <summary>
/// PlayerのBulletとボスとのCallBack
/// </summary>
class PBulletToBossCallBacks :
	public BaseCollisionCallBacks {
public:

	PBulletToBossCallBacks(CollisionManager* _collisionManager) : pCollisionManager_(_collisionManager) {};
	~PBulletToBossCallBacks() = default;

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;

	// 衝突開始
	void CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const boss) override;
	// 衝突中
	void CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const boss)  override;
	// 衝突終わり
	void CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const boss)  override;

public:		// accessor method

	void SetBulletManager(PlayerBulletManager* _manager) { pBulletManager_ = _manager; }
	void SetBoss(Boss* _boss) { pBoss_ = _boss; }

private:

	CollisionManager* pCollisionManager_;

	PlayerBulletManager* pBulletManager_;
	Boss* pBoss_;

	BaseParticles* hitBossExploadParticles_;
	BaseParticles* hitBossSmoke_;
	BaseParticles* hitBossSmokeBorn_;

	BaseParticles* hitSmoke_;
};

