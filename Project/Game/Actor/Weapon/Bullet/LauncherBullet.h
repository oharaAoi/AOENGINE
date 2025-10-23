#pragma once
#include "Game/Actor/Base/BaseBullet.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"

/// <summary>
/// 追尾をしない爆発弾
/// </summary>
class LauncherBullet :
	public BaseBullet {
public:

	LauncherBullet() = default;
	~LauncherBullet() override;

public:

	// 初期化
	void Init();
	// 更新
	void Update() override;
	
	/// <summary>
	/// 衝突時の処理
	/// </summary>
	/// <param name="other"></param>
	void OnCollision(ICollider* other);

	/// <summary>
	/// 弾の情報をリセットする
	/// </summary>
	/// <param name="_pos">: 弾の座標</param>
	/// <param name="_velocity">: 速度</param>
	void Reset(const Vector3& _pos, const Vector3& _velocity);

private:
	BaseParticles* burn_;
	BaseParticles* smoke_;

};

