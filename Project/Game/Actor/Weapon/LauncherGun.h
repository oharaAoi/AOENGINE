#pragma once
#include "Game/Actor/Weapon/BaseWeapon.h"
// Effect
#include "Game/Effects/GunFireParticles.h"

class LauncherGun :
	public BaseWeapon {
public:

	LauncherGun() = default;
	~LauncherGun() override = default;

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

	std::unique_ptr<GunFireParticles> gunFireParticles_;

};

