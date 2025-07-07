#pragma once
#include "Engine/Module/Components/AI/IBehaviorNode.h"

/// <summary>
/// Treeの根本となるクラス
/// </summary>
class BehaviorRootNode :
	public IBehaviorNode {
public:

	BehaviorRootNode();
	~BehaviorRootNode() = default;

	BehaviorStatus Execute() override;

private:



};

