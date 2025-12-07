#pragma once
#include "Engine/System/AI/GoalOriented/IOrientedGoal.h"

/// <summary>
/// Armorを貼る目標
/// </summary>
class DeployArmorOriented :
	public AI::IOrientedGoal {
public:

	DeployArmorOriented();
	~DeployArmorOriented() override = default;

public:

	// 更新処理
	void Update() override {};

	// スコアの計算
	float CalculationScore() override;

	// 編集
	void Debug_Gui() override;
};

