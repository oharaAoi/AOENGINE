#pragma once
#include "Game/Actor/Base/BaseBullet.h"
#include "Game/Actor/Boss/Bullet/BossBullets.h"

/// <summary>
/// Bossのミサイルを打つ処理
/// </summary>
class BossMissile :
	public BaseBullet {
public:

	BossMissile() = default;
	~BossMissile() override;

	void Init();
	void Update();
	void Draw() const;

	void Reset(const Vector3& pos, const Vector3& velocity, const Vector3& targetPosition, float bulletSpeed);

private :

	void Tracking();

public:

	void SetTargetPosition(const Vector3& pos) { targetPosition_ = pos; }

private :

	BossBulletType type_ = BossBulletType::MISSILE;

	float speed_;

	float trackingLength_;
	float trackingTimer_;
	float trackingTime_ = 1.f;

	bool finishTracking_;

};

