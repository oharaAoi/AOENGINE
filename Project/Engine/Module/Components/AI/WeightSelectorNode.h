#pragma once
#include <unordered_map>
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

	float EvaluateWeight() override { return 0; }

	void Debug_Gui() override;

private:

	std::unordered_map<uint32_t, float> weightMap_;

};

