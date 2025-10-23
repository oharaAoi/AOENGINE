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

