#pragma once
#include "Game/Actor/Base/BaseBullet.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"

/// <summary>
/// ロケットミサイル弾
/// </summary>
class RocketBullet :
	public BaseBullet {
public:

	RocketBullet() = default;
	~RocketBullet() override;

public:

	// 初期化
	void Init();
	// 更新
	void Update() override;
	
	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="other">: 衝突collider</param>
	void OnCollision(ICollider* other);

	/// <summary>
	/// 弾の情報をリセットする
	/// </summary>
	/// <param name="pos">: 弾の座標</param>
	/// <param name="target">: 目標の座標</param>
	/// <param name="bulletSpeed">: 弾の速度</param>
	void Reset(const Vector3& _pos, const Vector3& _target, float _bulletSpeed, float _trackingLength, float _trackingTime, float _trackingRaito);

private:

	void Tracking();

private:

	// 追尾に関する変数
	float trackingLength_;		// 長さ
	float trackingTimer_;		// 計測時間
	float trackingTime_ = 1.f;	// 追尾するまでの時間
	float trackingRaito_ = 0.8f;
	bool finishTracking_;		// 追尾を終了するか

	BaseParticles* burn_;
	BaseParticles* smoke_;
};

