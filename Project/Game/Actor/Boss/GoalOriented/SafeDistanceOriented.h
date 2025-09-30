#pragma once
#include "Engine/Module/Components/AI/GoalOriented/IOrientedGoal.h"
// game
#include "Game/Actor/Boss/GoalOriented/BossWorldState.h"

/// <summary>
/// 敵と距離を取る目標
/// </summary>
class SafeDistanceOriented :
	public IOrientedGoal {
public:

	SafeDistanceOriented();
	~SafeDistanceOriented() override = default;

	void Update() override {};

	bool IsGoal() override;

	float CalculationScore() override;

	void Debug_Gui() override;
};

