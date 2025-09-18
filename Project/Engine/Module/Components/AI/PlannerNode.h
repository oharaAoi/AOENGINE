#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include "Engine/Module/Components/AI/IBehaviorNode.h"
#include "Engine/Module/Components/AI/BehaviorTree.h"
#include "Engine/Module/Components/AI/State/IWorldState.h"
#include "Engine/Module/Components/AI/GoalOriented/IOrientedGoal.h"

/// <summary>
/// GoalまでのNodeたちを格納したNode
/// </summary>
class PlannerNode : 
	public IBehaviorNode {
public:

	PlannerNode(const std::unordered_map<std::string, std::shared_ptr<IBehaviorNode>>& _rootNodeCanTask,
				IWorldState* _worldState, 
				const std::vector<std::shared_ptr<IOrientedGoal>>& _goals);
	~PlannerNode() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<PlannerNode>(canTask_, pWorldState_, goalArray_);
	}

	json ToJson() override;

	BehaviorStatus Execute() override;

	float EvaluateWeight() override;

	void Debug_Gui() override;

	void SetGOBT(const std::string _orientedName, const std::string _treeFileName);

private:

	std::unique_ptr<BehaviorTree> tree_;
	std::unordered_map<std::string, std::shared_ptr<IBehaviorNode>> canTask_;
	std::vector<std::shared_ptr<IOrientedGoal>> goalArray_;
	IWorldState* pWorldState_ = nullptr;

	std::shared_ptr<IOrientedGoal> goal_;

	// 保存用
	std::string orientedName_;
	std::string treeFileName_;

};

