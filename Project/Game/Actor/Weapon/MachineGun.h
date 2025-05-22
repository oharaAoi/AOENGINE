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

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG

public:		// member method

	void Shot() override;

private:

	BaseParticles* gunFireParticles_;

};

