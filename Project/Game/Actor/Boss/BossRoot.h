#pragma once
// C++
#include <memory>
// Game
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/Bullet/BossBulletManager.h"
#include "Game/Actor/Player/Player.h"
#include "Game/UI/Boss/BossUIs.h"

/// <summary>
/// Boss関連の親
/// </summary>
class BossRoot {
public:

	BossRoot() = default;
	~BossRoot() = default;

public:

	// 初期化
	void Init();
	// 更新
	void Update();

public:

	Boss* GetBoss() { return boss_.get(); }

	BossBulletManager* GetBulletManager() { return bulletManager_.get(); }

	void SetPlayerPosition(const Math::Vector3& _position) { playerPosition_ = _position; }

	void SetUIs(BossUIs* _pBossUIs) { boss_->SetUIs(_pBossUIs); }

	void SetPlayer(Player* _player) { pPlayer_ = _player; }

	void SetTargetTransform(WorldTransform* _transform) { boss_->SetTargerTransform(_transform); }

private:

	std::unique_ptr<Boss> boss_;
	std::unique_ptr<BossBulletManager> bulletManager_;

	// 他クラス情報
	Math::Vector3 playerPosition_;
	Player* pPlayer_ = nullptr;

};

