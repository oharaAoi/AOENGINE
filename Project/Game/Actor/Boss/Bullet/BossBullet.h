#pragma once
#include "Game/Actor/Base/BaseBullet.h"
#include "Game/Actor/Boss/Bullet/BossBullets.h"
#include "Engine/System/ParticleSystem/Emitter/GpuParticleEmitter.h"
#include "Engine/Module/Components/Collider/BaseCollider.h"

/// <summary>
/// Bossの通常の弾を撃つ処理
/// </summary>
class BossBullet :
	public BaseBullet {
public:

	BossBullet() = default;
	~BossBullet() override;

public:

	// 初期化処理
	void Init();
	// 更新処理
	void Update() override;
	
	/// <summary>
	/// 球の情報をリセって
	/// </summary>
	/// <param name="pos">: 座標</param>
	/// <param name="velocity">: 速度</param>
	void Reset(const Math::Vector3& pos, const Math::Vector3& velocity);

public:

	/// <summary>
	/// 衝突時の処理
	/// </summary>
	/// <param name="other"></param>
	void OnCollision(AOENGINE::BaseCollider* other);

private:

	BossBulletType type_ = BossBulletType::Shot;

	AOENGINE::GpuParticleEmitter* trail_;
	float speed_;

	float deadLength_ = 200.f;
};

