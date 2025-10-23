#pragma once
// c++
#include <memory>
// game
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Bullet/PlayerBulletManager.h"
#include "Game/Actor/Weapon/MachineGun.h"
#include "Game/Actor/Weapon/LauncherGun.h"
#include "Game/Actor/Weapon/ShoulderMissile.h"
#include "Game/Actor/Weapon/Armors.h"

/// <summary>
/// Player関連を管理しているクラス
/// </summary>
class PlayerManager {
public:

	PlayerManager() = default;
	~PlayerManager() = default;

public:

	// 初期化
	void Init();
	// 更新
	void Update();
	// 遅れて更新
	void PostUpdate();

private:

	// 各アクションのチェック項目を判定する
	void CheckAction();
	
public:		// accessor method

	Player* GetPlayer() { return player_.get(); }

	PlayerBulletManager* GetBulletManager() { return bulletManager_.get(); }

private:

	std::unique_ptr<Player> player_;
	std::unique_ptr<PlayerBulletManager> bulletManager_;

	// weapon
	std::unique_ptr<MachineGun> machineGun_;
	std::unique_ptr<LauncherGun> launcherGun_;
	std::unique_ptr<ShoulderMissile> shoulderMissile_;

	std::unique_ptr<Armors> armors_;

};

