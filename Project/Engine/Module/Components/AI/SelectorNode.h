#pragma once
#include "Engine/Module/Components/AI/IBehaviorNode.h"

class SelectorNode :
	public IBehaviorNode {
public:

	SelectorNode();
	~SelectorNode() override = default;

	BehaviorStatus Execute() override;

private:
};

