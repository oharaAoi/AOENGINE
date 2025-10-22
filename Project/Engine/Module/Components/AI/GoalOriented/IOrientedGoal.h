#pragma once
#include <string>
#include "Engine/Module/Components/AI/State/IWorldState.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// 達成目標となる基底クラス
/// </summary>
class IOrientedGoal :
	public AttributeGui {
public: // コンストラクタ

	IOrientedGoal() = default;
	virtual ~IOrientedGoal() = default;

public:

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 目標の成否
	/// </summary>
	/// <returns>true : 達成 | false : 未達成</returns>
	virtual bool IsGoal() = 0;

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

	void SetPriority(int _priority) { priority_ = _priority; }
	int GetPriority() const { return priority_; }

	void SetWorldState(IWorldState* _worldState) { worldState_ = _worldState; }
	IWorldState* GetWorldState() const { return worldState_; }

protected:

	// 優先度
	int priority_;

	IWorldState* worldState_ = nullptr;

};
