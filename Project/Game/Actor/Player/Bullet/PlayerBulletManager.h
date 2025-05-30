#pragma once
// C++
#include <list>
#include <memory>
// Game
#include "Game/Actor/Player/Bullet/PlayerBullet.h"
#include <Module/Components/Collider/ICollider.h>

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

	PlayerBullet* SearchCollider(ICollider* collider);

public: // member method

	void AddBullet(const Vector3& pos, const Vector3& velocity, uint32_t type);

private:

	std::list<std::unique_ptr<PlayerBullet>> bulletList_;

};

