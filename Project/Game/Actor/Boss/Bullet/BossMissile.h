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

public:

	// 初期化
	void Init();
	// 更新
	void Update() override;
	
	/// <summary>
	/// ミサイルをリセットする
	/// </summary>
	/// <param name="pos">: 座標</param>
	/// <param name="velocity">: 速度</param>
	/// <param name="targetPosition">: ターゲットの座標</param>
	/// <param name="bulletSpeed">: 球の速度</param>
	/// <param name="firstSpeedRaito">: 初めの速度の割合</param>
	/// <param name="trackingRaito">: 追尾性能の値</param>
	/// <param name="isTracking">: 追尾を行うか</param>
	void Reset(const Math::Vector3& pos, const Math::Vector3& velocity, const Math::Vector3& targetPosition, float bulletSpeed, float firstSpeedRaito,
			   float trackingRaito, bool isTracking);

	/// <summary>
	/// 衝突時の処理
	/// </summary>
	/// <param name="other"></param>
	void OnCollision(BaseCollider* other);

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

	void SetTargetPosition(const Math::Vector3& pos) { targetPosition_ = pos; }

private :

	BossBulletType type_ = BossBulletType::Missile;

	// 追尾に関する変数
	float trackingLength_ = 5.0f;		// 長さ
	float trackingTimer_ = 0;		// 計測時間
	float trackingTime_ = 1.f;	// 追尾するまでの時間

	bool finishTracking_;		// 追尾を終了するか

	float trackingRaito_;		// 追従を行う割合

	float deadLength_ = 200.0f;

	float firstSpeedRaito_;		// 初期速度の割合
	float targetSpeed_;			// 目標の速度
	Timer accelTimer_ = Timer();

	BaseParticles* burn_;
	BaseParticles* smoke_;
	BaseParticles* shotFrea_;

};

