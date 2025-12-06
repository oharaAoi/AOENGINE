#pragma once
// Engine
#include "Engine/Module/Components/Effect/BaseParticles.h"
// Game
#include "Game/Actor/Weapon/BaseWeapon.h"

/// <summary>
/// マシンガン
/// </summary>
class MachineGun :
	public BaseWeapon {
public:

	MachineGun() = default;
	~MachineGun() override = default;

public:

	// 終了
	void Finalize() override;
	// 初期化
	void Init() override;
	// 編集
	void Debug_Gui() override;

public:		// member method

	// 攻撃
	bool Attack(const AttackContext& cxt) override;

private:

	AOENGINE::BaseParticles* gunFireParticles_;
	Math::Vector3 offset_;
};

