#pragma once
#include "Game/Actor/Base/BaseBullet.h"
#include "Game/Actor/Boss/Bullet/BossBullets.h"

/// <summary>
/// Bossの通常の弾を撃つ処理
/// </summary>
class BossBullet :
	public BaseBullet {
public:

	BossBullet() = default;
	~BossBullet() override;

	void Init();
	void Update() override;
	
	void Reset(const Vector3& pos, const Vector3& velocity, float bulletSpeed);

private:

	BossBulletType type_ = BossBulletType::SHOT;

	float speed_;

};

