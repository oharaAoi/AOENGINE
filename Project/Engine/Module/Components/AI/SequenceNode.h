#pragma once
#include "Engine/Module/Components/AI/IBehaviorNode.h"

class SequenceNode :
	public IBehaviorNode {
public:

	SequenceNode();
	~SequenceNode() override = default;

	BehaviorStatus Execute() override;

	void Debug_Gui() override {};

};

