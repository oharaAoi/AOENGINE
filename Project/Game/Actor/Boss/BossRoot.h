#pragma once
// C++
#include <memory>
// Game
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/Bullet/BossBulletManager.h"

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

private:

	std::unique_ptr<Boss> boss_;
	std::unique_ptr<BossBulletManager> bulletManager_;

};

