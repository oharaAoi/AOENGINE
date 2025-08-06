#pragma once
#include "Game/Actor/Base/BaseBullet.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"

class RocketBullet :
	public BaseBullet {
public:

	RocketBullet() = default;
	~RocketBullet() override;

	void Init();

	void Update() override;

	void OnCollision(ICollider* other);

	void Reset(const Vector3& pos, const Vector3& target, float bulletSpeed);

private:

	void Tracking();

private:

	// 追尾に関する変数
	float trackingLength_;		// 長さ
	float trackingTimer_;		// 計測時間
	float trackingTime_ = 1.f;	// 追尾するまでの時間
	bool finishTracking_;		// 追尾を終了するか
	float trackingRaito_ = 0.8f;

	BaseParticles* burn_;
	BaseParticles* smoke_;
};

