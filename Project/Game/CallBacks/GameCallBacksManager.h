#pragma once
#include <memory>
#include "Engine/System/Manager/CollisionManager.h"
#include "Game/Actor/Player/PlayerManager.h"
#include "Game/Actor/Boss/BossRoot.h"
#include "Game/Actor/Enemy/EnemyManager.h"
#include "Game/WorldObject/Floor.h"
#include "Game/CallBacks/PBulletToBossCallBacks.h"
#include "Game/CallBacks/PBulletToEnemyCallBacks.h"
#include "Game/CallBacks/PlayerToGroundCallBacks.h"
#include "Game/CallBacks/BBulletToPlayerCallBacks.h"
#include "Game/CallBacks/BBulletToGroundCallBacks.h"
#include "Game/CallBacks/RocketToBossCallBacks.h"
#include "Game/CallBacks/AttackArmorToPlayerCallBacks.h"
#include "Game/CallBacks/EnemySearchToPlayerCallBacks.h"

/// <summary>
/// CallBackをまとめたクラス
/// </summary>
class GameCallBacksManager {
public:

	GameCallBacksManager() = default;
	~GameCallBacksManager() = default;

public:

	// 初期化
	void Init(AOENGINE::CollisionManager* _manager);

	/// <summary>
	/// 後で確実に消す
	/// </summary>
	void Update();

public:		// accessor method

	void SetPlayerManager(PlayerManager* _manager) { pPlayerManager_ = _manager; }
	void SetBossRoot(BossRoot* _boss) { pBossRoot_ = _boss; }
	void SetGround(Floor* _floor) { pFloor_ = _floor; }
	void SetEnemyManager(EnemyManager* enemyManager) { pEnemyManager_ = enemyManager; }

private:

	// collisionManager
	AOENGINE::CollisionManager* pCollisionManager_;

	// objectポインタ
	PlayerManager* pPlayerManager_ = nullptr;
	BossRoot* pBossRoot_ = nullptr;
	Floor* pFloor_ = nullptr;
	EnemyManager* pEnemyManager_ = nullptr;

	// callBacks
	std::unique_ptr<PBulletToBossCallBacks> pBulletToBoss_;
	std::unique_ptr<PBulletToEnemyCallBacks> pBulletToEnemy_;
	std::unique_ptr<BBulletToPlayerCallBacks> pBBulletToPlayer_;
	std::unique_ptr<BBulletToGroundCallBacks> pBBulletToGround_;
	std::unique_ptr<PlayerToGroundCallBacks> pPlayerToGround_;
	std::unique_ptr<RocketToBossCallBacks> pRocketToBoss_;
	std::unique_ptr<AttackArmorToPlayerCallBacks> pAttackArmorToPlayer_;
	std::unique_ptr<EnemySearchToPlayerCallBacks> enemySearchToPlayer_;

};

