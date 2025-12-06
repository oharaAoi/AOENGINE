#pragma once
#include "Game/Actor/Base/BaseBullet.h"
#include "Engine/System/ParticleSystem/Emitter/GpuParticleEmitter.h"

enum class PlayerBulletType{
	MachineGun,
	Launcher,
};

/// <summary>
/// Playerの基本の弾
/// </summary>
class PlayerBullet :
	public BaseBullet {
public:

	PlayerBullet() = default;
	~PlayerBullet() override;

public:

	// 初期化
	void Init();
	// 更新
	void Update() override;
	
public:
	void SetType(uint32_t type) { bulletType = type; }

	uint32_t GetType() const { return bulletType; }

public:

	void OnCollision(AOENGINE::BaseCollider* other);

	/// <summary>
	/// 座標や速度の設定
	/// </summary>
	/// <param name="pos">: 座標</param>
	/// <param name="velocity">: 速度</param>
	void Reset(const Math::Vector3& pos, const Math::Vector3& velocity);

private:

	uint32_t bulletType = (int)PlayerBulletType::MachineGun;
	GpuParticleEmitter* trail_;

	float deadDistance_ = 200.0f;
};

