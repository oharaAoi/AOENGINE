#pragma once
// c++
#include <memory>
// game
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Bullet/PlayerBulletManager.h"

class PlayerManager {
public:

	PlayerManager() = default;
	~PlayerManager() = default;

	void Init();
	void Update();
	void Draw() const;

public:		// accessor method

	Player* GetPlayer() { return player_.get(); }

private:

	std::unique_ptr<Player> player_;
	std::unique_ptr<PlayerBulletManager> bulletManager_;

};

