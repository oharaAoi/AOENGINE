#pragma once
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"
#include "Engine/System/Manager/CollisionManager.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Enemy/EnemyManager.h"

/// <summary>
/// Enemyの索敵コライダーとプレイヤーのコールバッククラス
/// </summary>
class EnemySearchToPlayerCallBacks :
	public AOENGINE::BaseCollisionCallBacks {
public:

	EnemySearchToPlayerCallBacks(AOENGINE::CollisionManager* _collisionManager) : pCollisionManager_(_collisionManager) {};
	~EnemySearchToPlayerCallBacks() = default;

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;

	// 衝突開始
	void CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const player, [[maybe_unused]] AOENGINE::BaseCollider* const enemy) override;
	// 衝突中
	void CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const player, [[maybe_unused]] AOENGINE::BaseCollider* const enemy)  override;
	// 衝突終わり
	void CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const player, [[maybe_unused]] AOENGINE::BaseCollider* const enemy)  override;

public:		// accessor method

	void SetPlayer(Player* _player) { pPlayer_ = _player; }

	void SetEnemyManager(EnemyManager* enemyManager) { pEnemyManager_ = enemyManager; }

private:

	AOENGINE::CollisionManager* pCollisionManager_;

	Player* pPlayer_ = nullptr;
	EnemyManager* pEnemyManager_ = nullptr;
};

