#pragma once
// C++
#include <memory>
// Game
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/Bullet/BossBulletManager.h"
#include "Game/UI/Boss/BossUIs.h"

/// <summary>
/// Boss関連の親
/// </summary>
class BossRoot {
public:

	BossRoot() = default;
	~BossRoot() = default;

	void Init();
	void Update();
	void Draw() const;

public:

	Boss* GetBoss() { return boss_.get(); }

	BossBulletManager* GetBulletManager() { return bulletManager_.get(); }

	void SetPlayerPosition(const Vector3& _position) { playerPosition_ = _position; }

	void SetUIs(BossUIs* _pBossUIs) { boss_->SetUIs(_pBossUIs); }

private:

	std::unique_ptr<Boss> boss_;
	std::unique_ptr<BossBulletManager> bulletManager_;

	// 他クラス情報
	Vector3 playerPosition_;

};

