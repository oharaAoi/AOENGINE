#pragma once
#include <string>
#include "Engine/System/AI/State/Blackboard.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

namespace AI {

/// <summary>
/// 達成目標となる基底クラス
/// </summary>
class IOrientedGoal :
	public AOENGINE::AttributeGui {
public: // コンストラクタ

	IOrientedGoal() = default;
	virtual ~IOrientedGoal() = default;

public:

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// Scoreを計算する
	/// </summary>
	/// <returns></returns>
	virtual float CalculationScore() = 0;

	/// <summary>
	/// 編集処理
	/// </summary>
	virtual void Debug_Gui() override = 0;

public:

	void SetPriority(float _priority) { priority_ = _priority; }
	float GetPriority() const { return priority_; }

	void SetBlackboard(Blackboard* _worldState) { blackboard_ = _worldState; }
	Blackboard* GetBlackboard() const { return blackboard_; }

protected:

	// 優先度
	float priority_ = 0;

	Blackboard* blackboard_ = nullptr;

	// 最低補正値
	static constexpr float sMinimumCorrectionCalue_ = 0.01f;

};

}