#pragma once
#include <string>
#include "Engine/Module/Components/AI/State/IWorldState.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// 達成目標となる基底クラス
/// </summary>
class IOrientedGoal :
	public AttributeGui {
public:

	IOrientedGoal() = default;
	virtual ~IOrientedGoal() = default;

	virtual void Update() = 0;

	virtual bool IsGoal() = 0;

	virtual float CalculationScore() = 0;

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
