#pragma once
#include "Game/Actor/Base/BaseBullet.h"
#include "Game/Actor/Boss/Bullet/BossBullets.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/Utilities/Timer.h"

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
	
	void Reset(const Vector3& pos, const Vector3& velocity, const Vector3& targetPosition, float bulletSpeed, float firstSpeedRaito,
			   float trackingRaito, bool isTracking);

	void OnCollision(ICollider* other);

private :

	/// <summary>
	/// 追従する
	/// </summary>
	void Tracking();

	/// <summary>
	/// 加速させる
	/// </summary>
	void Accelerate();

public:

	void SetTargetPosition(const Vector3& pos) { targetPosition_ = pos; }

private :

	BossBulletType type_ = BossBulletType::MISSILE;

	// 追尾に関する変数
	float trackingLength_;		// 長さ
	float trackingTimer_;		// 計測時間
	float trackingTime_ = 1.f;	// 追尾するまでの時間

	bool finishTracking_;		// 追尾を終了するか

	float trackingRaito_;		// 追従を行う割合

	float firstSpeedRaito_;		// 初期速度の割合
	float targetSpeed_;			// 目標の速度
	Timer accelTimer_ = Timer();

	BaseParticles* burn_;
	BaseParticles* smoke_;

};

