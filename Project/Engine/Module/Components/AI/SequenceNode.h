#pragma once
#include "Engine/Module/Components/AI/IBehaviorNode.h"

/// <summary>
/// 一番左の子が成功したら次の子へと遷移するNode
/// </summary>
class SequenceNode :
	public IBehaviorNode {
public:

	SequenceNode();
	~SequenceNode() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<SequenceNode>(*this);
	}

	json ToJson() override;

	BehaviorStatus Execute() override;

	float EvaluateWeight() override;

	void Debug_Gui() override;

};

