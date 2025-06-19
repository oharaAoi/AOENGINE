#pragma once
#include "Game/Actor/Base/BaseBullet.h"
#include "Game/Actor/Boss/Bullet/BossBullets.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"

/// <summary>
/// Bossのミサイルを打つ処理
/// </summary>
class BossMissile :
	public BaseBullet {
public:

	BossMissile() = default;
	~BossMissile() override;

	void Init();
	void Update() override;
	
	void Reset(const Vector3& pos, const Vector3& velocity, const Vector3& targetPosition, float bulletSpeed, 
			   float trackingRaito, bool isTracking);

private :

	void Tracking();

public:

	void SetTargetPosition(const Vector3& pos) { targetPosition_ = pos; }

private :

	BossBulletType type_ = BossBulletType::MISSILE;

	// 追尾に関する変数
	float trackingLength_;		// 長さ
	float trackingTimer_;		// 計測時間
	float trackingTime_ = 1.f;	// 追尾するまでの時間

	bool finishTracking_;		// 追尾を終了するか

	float trackingRaito_;

	BaseParticles* burn_;
	BaseParticles* smoke_;

};

