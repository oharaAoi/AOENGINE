#pragma once
#include "Engine/Components/EventHandlers/BaseCollisionCallBacks.h"
#include "Engine/System/Manager/CollisionManager.h"
#include "Game/Actor/Player/Bullet/PlayerBulletManager.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Effects/HitBossExploadParticles.h"
#include "Game/Effects/HitBossSmoke.h"
#include "Game/Effects/HitBossSmokeBorn.h"

/// <summary>
/// PlayerのBulletとボスとのCallBack
/// </summary>
class PBulletToBossCallBacks :
	public BaseCollisionCallBacks {
public:

	PBulletToBossCallBacks(CollisionManager* _collisionManager) : pCollisionManager_(_collisionManager) {};
	~PBulletToBossCallBacks() = default;

	void Init() override;

	void Update() override;

	void CollisionEnter([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) override;
	void CollisionStay([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss)  override;
	void CollisionExit([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss)  override;

public:		// accessor method

	void SetBulletManager(PlayerBulletManager* _manager) { pBulletManager_ = _manager; }
	void SetBoss(Boss* _boss) { pBoss_ = _boss; }

private:

	CollisionManager* pCollisionManager_;

	PlayerBulletManager* pBulletManager_;
	Boss* pBoss_;

	std::unique_ptr<HitBossExploadParticles> hitBossExploadParticles_;
	std::unique_ptr<HitBossSmoke> hitBossSmoke_;
	std::unique_ptr<HitBossSmokeBorn> hitBossSmokeBorn_;

};

