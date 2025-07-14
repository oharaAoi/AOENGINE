#pragma once
#include "Engine/Module/Components/AI/IBehaviorNode.h"

/// <summary>
/// 重みの計算を行ってから選択をするセレクタ
/// </summary>
class WeightSelectorNode :
	public IBehaviorNode {
public:

	WeightSelectorNode();
	~WeightSelectorNode() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<WeightSelectorNode>(*this);
	}

	BehaviorStatus Execute() override;

	void Debug_Gui() override;

};

