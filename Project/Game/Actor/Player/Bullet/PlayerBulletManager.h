#pragma once
// C++
#include <list>
// Game
#include "Game/Actor/Player/Bullet/PlayerBullet.h"

/// <summary>
/// PlayerのBulletを管理するクラス
/// </summary>
class PlayerBulletManager {
public:

	PlayerBulletManager() = default;
	~PlayerBulletManager();

	void Init();
	void Update();
	void Draw() const;

public: // member method

	void AddBullet(const Vector3& pos, const Vector3& velocity);

private:

	std::list<PlayerBullet> bulletList_;

};

