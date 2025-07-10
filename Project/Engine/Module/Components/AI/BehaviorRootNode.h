#pragma once
#include "Engine/Module/Components/AI/IBehaviorNode.h"
#include <memory>

/// <summary>
/// Treeの根本となるクラス
/// </summary>
class BehaviorRootNode :
	public IBehaviorNode {
public:

	BehaviorRootNode();
	~BehaviorRootNode() = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BehaviorRootNode>(*this);
	}

	BehaviorStatus Execute() override;

	void Debug_Gui() override;

private:



};

