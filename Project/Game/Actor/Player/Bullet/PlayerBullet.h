#pragma once
#include "Game/Actor/Base/BaseBullet.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/System/ParticleSystem/Emitter/GpuParticleEmitter.h"

enum PlayerBulletType {
	MACHINEGUN,
	LANCHER,
};

/// <summary>
/// Playerの基本の弾
/// </summary>
class PlayerBullet :
	public BaseBullet {
public:

	PlayerBullet() = default;
	~PlayerBullet() override;

	void Init();
	void Update() override;
	
	void SetType(uint32_t type) { bulletType = type; }

	uint32_t GetType() const { return bulletType; }

public:

	void OnCollision(ICollider* other);

private:

	uint32_t bulletType = MACHINEGUN;
	GpuParticleEmitter* trail_;

};

