#pragma once
// Engine
#include "Engine/Module/Components/Effect/BaseParticles.h"
// Game
#include "Game/Actor/Weapon/BaseWeapon.h"

class LauncherGun :
	public BaseWeapon {
public:

	LauncherGun() = default;
	~LauncherGun() override = default;

	void Finalize() override;
	void Init() override;
	
	void Debug_Gui() override;

public:		// member method

	void Attack(const AttackContext& cxt) override;

private:

	BaseParticles* gunFireParticles_;
	Vector3 offset_;

};

