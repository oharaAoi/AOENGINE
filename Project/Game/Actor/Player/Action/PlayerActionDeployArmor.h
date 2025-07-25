#pragma once
#include "Game/Actor/Base/BaseAction.h"
#include "Engine/System/Input/Input.h"

class Player;


class PlayerActionDeployArmor :
	public BaseAction<Player> {
public:

	PlayerActionDeployArmor() = default;
	~PlayerActionDeployArmor() override = default;

	void Debug_Gui() override;

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

private:

	Input* pInput_;

};

