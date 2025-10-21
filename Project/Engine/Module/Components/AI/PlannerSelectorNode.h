#pragma once
#include "Engine/Module/Components/AI/IBehaviorNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class PlannerSelectorNode :
	public IBehaviorNode {
public:

	PlannerSelectorNode();
	~PlannerSelectorNode() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<PlannerSelectorNode>(*this);
	}

	json ToJson() override;

	BehaviorStatus Execute() override;

	float EvaluateWeight() override;

	void Debug_Gui() override;

private:

	/// <summary>
	/// 評価値を表示する
	/// </summary>
	void PriorityDisplay();

private:

	std::unordered_map<uint32_t, float> priorityMap_;
	bool reset_ = false;

};

