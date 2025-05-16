#pragma once
// C++
#include <list>
#include <memory>
// Game
#include "Game/Actor/Base/BaseBullet.h"

enum class BossBulletType {
	MISSILE,
};

/// <summary>
/// BossのBulletを管理するクラス
/// </summary>
class BossBulletManager {
public:

	BossBulletManager() = default;
	~BossBulletManager() = default;

	void Init();
	void Update();
	void Draw() const;

public: // member method

	void AddBullet(const Vector3& pos, const Vector3& velocity, BossBulletType type);

private:

	std::list<std::unique_ptr<BaseBullet>> bulletList_;

};

