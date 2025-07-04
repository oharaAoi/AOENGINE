#pragma once
// c++
#include <memory>
// game
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Bullet/PlayerBulletManager.h"
#include "Game/Actor/Weapon/MachineGun.h"
#include "Game/Actor/Weapon/LauncherGun.h"

class PlayerManager {
public:

	PlayerManager() = default;
	~PlayerManager() = default;

	void Init();
	void Update();
	void Draw() const;

public:		// accessor method

	Player* GetPlayer() { return player_.get(); }

	PlayerBulletManager* GetBulletManager() { return bulletManager_.get(); }

private:

	std::unique_ptr<Player> player_;
	std::unique_ptr<PlayerBulletManager> bulletManager_;

	// weapon
	std::unique_ptr<MachineGun> machineGun_;
	std::unique_ptr<LauncherGun> launcherGun_;

};

