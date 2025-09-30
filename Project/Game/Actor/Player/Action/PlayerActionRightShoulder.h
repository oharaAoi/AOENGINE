#pragma once
#include "Game/Actor/Base/BaseAction.h"
#include "Game/Actor/Weapon/ShoulderMissile.h"
#include "Engine/System/Input/Input.h"

class Player;

class PlayerActionRightShoulder :
	public BaseAction<Player> {
public:

public:

	PlayerActionRightShoulder() = default;
	~PlayerActionRightShoulder() override {};

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

	void Debug_Gui() override;

private:

	void Shot();

private:

	Input* pInput_;
	ShoulderMissile* pWeapon_;

	bool isFinish_;
};

