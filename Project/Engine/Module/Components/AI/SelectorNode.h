#pragma once
#include "Engine/Module/Components/AI/IBehaviorNode.h"

class SelectorNode :
	public IBehaviorNode {
public:

	SelectorNode();
	~SelectorNode() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<SelectorNode>(*this);
	}

	BehaviorStatus Execute() override;

	float EvaluateWeight() override { return 0; }

	void Debug_Gui() override;
};