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
public: // コンストラクタ

	PlannerNode(const std::unordered_map<std::string, std::shared_ptr<IBehaviorNode>>& _rootNodeCanTask,
				IWorldState* _worldState, 
				const std::vector<std::shared_ptr<IOrientedGoal>>& _goals);
	~PlannerNode() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<PlannerNode>(canTask_, pWorldState_, goalArray_);
	}

public:

	// jsonへ
	json ToJson() override;

	// 実行
	BehaviorStatus Execute() override;

	// weight値を算出
	float EvaluateWeight() override;

	// 編集処理
	void Debug_Gui() override;

	/// <summary>
	/// 目標の設定
	/// </summary>
	/// <param name="_orientedName">: 目標の名前</param>
	/// <param name="_treeFileName">: 所有するtreeの名前</param>
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

