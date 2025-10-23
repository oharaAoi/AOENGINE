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

public:

	// 更新処理
	void Update() override {};

	// 目標達成の判別
	bool IsGoal() override;

	// スコアの計算
	float CalculationScore() override;

	// 編集
	void Debug_Gui() override;
};

