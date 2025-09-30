#pragma once
#include "Engine/Module/Components/AI/GoalOriented/IOrientedGoal.h"

/// <summary>
/// Armorを貼る目標
/// </summary>
class DeployArmorOriented :
	public IOrientedGoal {
public:

	DeployArmorOriented();
	~DeployArmorOriented() override = default;

	void Update() override {};

	bool IsGoal() override;

	float CalculationScore() override;

	void Debug_Gui() override;
};

