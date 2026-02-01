#pragma once
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"
#include "Engine/System/Manager/CollisionManager.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
// Game
#include "Game/Actor/Player/Bullet/PlayerBulletManager.h"
#include "Game/Actor/Enemy/EnemyManager.h"

/// <summary>
/// Playerのバレットとエネミーのコールバック
/// </summary>
class PBulletToEnemyCallBacks :
	public AOENGINE::BaseCollisionCallBacks {
public:

	PBulletToEnemyCallBacks(AOENGINE::CollisionManager* _collisionManager) : pCollisionManager_(_collisionManager) {};
	~PBulletToEnemyCallBacks() = default;

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;

	// 衝突開始
	void CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const enemy) override;
	// 衝突中
	void CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const enemy)  override;
	// 衝突終わり
	void CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const enemy)  override;
public:		// accessor method

	void SetBulletManager(PlayerBulletManager* _manager) { pBulletManager_ = _manager; }
	void SetEnemyManager(EnemyManager* enemyManager) { pEnemyManager_ = enemyManager; }

private:

	AOENGINE::CollisionManager* pCollisionManager_;

	PlayerBulletManager* pBulletManager_;
	EnemyManager* pEnemyManager_;

	AOENGINE::BaseParticles* hitBossExplodeParticles_;
	AOENGINE::BaseParticles* hitBossSmoke_;
	AOENGINE::BaseParticles* hitBossSmokeBorn_;

	AOENGINE::BaseParticles* hitSmoke_;
};

