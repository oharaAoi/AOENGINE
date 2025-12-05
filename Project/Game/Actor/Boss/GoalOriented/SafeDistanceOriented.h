#pragma once
#include "Engine/System/AI/GoalOriented/IOrientedGoal.h"


/// <summary>
/// 敵と距離を取る目標
/// </summary>
class SafeDistanceOriented :
	public IOrientedGoal {
public:

	SafeDistanceOriented();
	~SafeDistanceOriented() override = default;

public:

	// 更新処理
	void Update() override {};

	// スコアの計算
	float CalculationScore() override;

	// 編集
	void Debug_Gui() override;
};

