#pragma once
#include "Engine/Module/Components/AI/GoalOriented/IOrientedGoal.h"
// game
#include "Game/Actor/Boss/GoalOriented/BossWorldState.h"

/// <summary>
/// 敵を倒す目標
/// </summary>
class TargetDeadOriented :
	public IOrientedGoal {
public:

	TargetDeadOriented();
	~TargetDeadOriented() override = default;

	void Update() override;

	bool IsGoal() override;

	float CalculationScore() override;

};

