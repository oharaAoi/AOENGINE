#pragma once
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"
#include "Engine/System/Manager/CollisionManager.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
// Game
#include "Game/Actor/Boss/Bullet/BossBulletManager.h"
#include "Game/Actor/Player/Player.h"

/// <summary>
/// BossのBulletとPlayerのCallBack
/// </summary>
class BBulletToPlayerCallBacks :
	public BaseCollisionCallBacks {
public:

	BBulletToPlayerCallBacks(CollisionManager* _collisionManager) : pCollisionManager_(_collisionManager) {};
	~BBulletToPlayerCallBacks() = default;

	void Init() override;

	void Update() override;

	void CollisionEnter([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const player) override;
	void CollisionStay([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const player)  override;
	void CollisionExit([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const player)  override;

public:

	void SetBossBulletManager(BossBulletManager* _manager) { pBossBulletManager_ = _manager; }

	void SetPlayer(Player* _player) { pPlayer_ = _player; }

private:

	CollisionManager* pCollisionManager_;

	BossBulletManager* pBossBulletManager_;
	Player* pPlayer_;

	BaseParticles* hitEffect_;

};

