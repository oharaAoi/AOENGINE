#pragma once
// C++
#include <list>
#include <memory>
// Game
#include "Game/Actor/Player/Bullet/PlayerBullet.h"
#include "Game/Effects/HitBossExploadParticles.h"
#include "Game/Effects/HitBossSmoke.h"
#include "Game/Effects/HitBossSmokeBorn.h"
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

	void CollisionToBoss(const Vector3& bossPos);

	PlayerBullet* SearchCollider(ICollider* collider);

public: // member method

	void AddBullet(const Vector3& pos, const Vector3& velocity);

private:

	std::list<std::unique_ptr<PlayerBullet>> bulletList_;

	std::unique_ptr<HitBossExploadParticles> hitBossExploadParticles_;
	std::unique_ptr<HitBossSmoke> hitBossSmoke_;
	std::unique_ptr<HitBossSmokeBorn> hitBossSmokeBorn_;
};

