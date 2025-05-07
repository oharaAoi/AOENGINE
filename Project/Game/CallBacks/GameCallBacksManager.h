#pragma once
#include <memory>
#include "Engine/System/Manager/CollisionManager.h"
#include "Game/Actor/Player/PlayerManager.h"
#include "Game/CallBacks/PBulletToBossCallBacks.h"

/// <summary>
/// CallBackをまとめたクラス
/// </summary>
class GameCallBacksManager {
public:

	GameCallBacksManager() = default;
	~GameCallBacksManager() = default;

	void Init(CollisionManager* _manager);

	/// <summary>
	/// 後で確実に消す
	/// </summary>
	void Update();

public:		// accessor method

	void SetPlayerManager(PlayerManager* _manager) { pPlayerManager_ = _manager; }
	void SetBoss(Boss* _boss) { pBoss_ = _boss; }

private:

	// collisionManager
	CollisionManager* pCollisionManager_;

	// objectポインタ
	PlayerManager* pPlayerManager_ = nullptr;
	Boss* pBoss_ = nullptr;

	// callBacks
	std::unique_ptr<PBulletToBossCallBacks> pBulletToBoss_;

};

