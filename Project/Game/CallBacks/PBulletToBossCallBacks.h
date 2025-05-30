#pragma once
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"
#include "Engine/System/Manager/CollisionManager.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
// Game
#include "Game/Actor/Player/Bullet/PlayerBulletManager.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Effects/HitExploade.h"

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

	void Draw() const;

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

	BaseParticles* hitBossExploadParticles_;
	BaseParticles* hitBossSmoke_;
	BaseParticles* hitBossSmokeBorn_;

	BaseParticles* hitSmoke_;

	std::list<std::unique_ptr<HitExplode>> hitExplodeList_;

};

