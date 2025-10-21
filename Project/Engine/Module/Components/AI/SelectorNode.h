#pragma once
#include "Engine/Module/Components/AI/IBehaviorNode.h"

/// <summary>
/// 左から実行をし、 失敗したら次へ成功したら親へ戻るNode
/// </summary>
class SelectorNode :
	public IBehaviorNode {
public:

	SelectorNode();
	~SelectorNode() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<SelectorNode>(*this);
	}

	json ToJson() override;

	BehaviorStatus Execute() override;

	float EvaluateWeight() override;

	void Debug_Gui() override;
};