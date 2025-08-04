#pragma once
#include "Engine/Module/Components/AI/IBehaviorNode.h"

class SequenceNode :
	public IBehaviorNode {
public:

	SequenceNode();
	~SequenceNode() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<SequenceNode>(*this);
	}

	BehaviorStatus Execute() override;

	float EvaluateWeight() override;

	void Debug_Gui() override;

};

