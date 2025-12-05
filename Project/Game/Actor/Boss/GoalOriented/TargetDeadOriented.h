#pragma once
#include "Engine/System/AI/GoalOriented/IOrientedGoal.h"

/// <summary>
/// 敵を倒す目標
/// </summary>
class TargetDeadOriented :
	public IOrientedGoal {
public:

	TargetDeadOriented();
	~TargetDeadOriented() override = default;

public:

	// 更新処理
	void Update() override {};

	// スコアの計算
	float CalculationScore() override;

	// 編集
	void Debug_Gui() override;

};

