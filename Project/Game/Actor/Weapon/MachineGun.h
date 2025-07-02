#pragma once
// Engine
#include "Engine/Module/Components/Effect/BaseParticles.h"
// Game
#include "Game/Actor/Weapon/BaseWeapon.h"

class MachineGun :
	public BaseWeapon {
public:

	MachineGun() = default;
	~MachineGun() override = default;

	void Finalize() override;
	void Init() override;
	void Update() override;
	void Draw() const override;

	void Debug_Gui() override;

public:		// member method

	void Shot(const Vector3& direction, uint32_t type) override;

private:

	BaseParticles* gunFireParticles_;

};

