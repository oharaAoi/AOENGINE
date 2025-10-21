#include "PlannerNode.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

namespace fs = std::filesystem;

PlannerNode::PlannerNode(const std::unordered_map<std::string, std::shared_ptr<IBehaviorNode>>& _rootNodeCanTask, 
						 IWorldState* _worldState,
						 const std::vector<std::shared_ptr<IOrientedGoal>>& _goals) {
	canTask_ = _rootNodeCanTask;
	pWorldState_ = _worldState;
	goalArray_ = _goals;

	tree_ = std::make_unique<BehaviorTree>();
	tree_->Init();
	tree_->SetCanTaskMap(_rootNodeCanTask);
	tree_->SetWorldState(pWorldState_);
	tree_->SetName("plannerTree");

	color_ = ImColor(255, 99, 71);
	baseColor_ = color_;
	type_ = NodeType::Planner;
	SetNodeName("Planner");
}

json PlannerNode::ToJson() {
	if (goal_) {
		orientedName_ = goal_->GetName();
	}

	json item;
	item["name"] = node_.name;
	item["nodeType"] = static_cast<int>(type_);
	item["nodePos"] = json{ {"x", pos_.x}, {"y", pos_.y} };
	item["children"] = json::array();
	item["coolTime"] = coolTime_;
	item["orientedName"] = orientedName_;
	item["treeFileName"] = treeFileName_;

	for (const auto& child : children_) {
		item["children"].push_back(child->ToJson());
	}
	return item;
}

BehaviorStatus PlannerNode::Execute() {
	bool isExecute = tree_->Run();

	if (goal_) {
		if (goal_->IsGoal()) {
			return BehaviorStatus::Success;
		}
	}

	if (isExecute) {
		return BehaviorStatus::Running;
	} else {
		return BehaviorStatus::Failure;
	}
}

float PlannerNode::EvaluateWeight() {
	if (goal_) {
		goal_->Update();
		return goal_->CalculationScore();
	}
	return 0.0f;
}

void PlannerNode::Debug_Gui() {
	EditNodeName();

	// Treeの編集
	if (ImGui::TreeNode("Have Tree")) {
		tree_->Edit();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Select Tree")) {
		std::string loadFilePath;
		if (ButtonOpenDialog("Select Tree", "Select_Tree", "SelectTree", ".json", loadFilePath)) {
			treeFileName_ = loadFilePath;
		}
		ImGui::TreePop();
	}
	ImGui::Separator();
	// 現在選択中のインデックス
	static int currentGoalIndex = -1;
	if (ImGui::BeginCombo("Select Goal",
						  (currentGoalIndex >= 0 && currentGoalIndex < goalArray_.size())
						  ? goalArray_[currentGoalIndex]->GetName().c_str()
						  : "None")) {
		for (int i = 0; i < goalArray_.size(); ++i) {
			const bool isSelected = (currentGoalIndex == i);
			if (ImGui::Selectable(goalArray_[i]->GetName().c_str(), isSelected)) {
				currentGoalIndex = i; // 選択変更
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	// 目標を設定する
	if (goal_) {
		std::string currentOriented = "Oriented : " + goal_->GetName();
		ImGui::Text(currentOriented.c_str());
		goal_->Debug_Gui();
	}

	// 選択されたゴールにアクセス
	if (currentGoalIndex >= 0 && currentGoalIndex < goalArray_.size()) {
		goal_ = goalArray_[currentGoalIndex];
	}
}

void PlannerNode::SetGOBT(const std::string _orientedName, const std::string _treeFileName) {
	orientedName_ = _orientedName;
	treeFileName_ = _treeFileName;
	// goalを設定する
	if (orientedName_ != "") {
		for (auto goal : goalArray_) {
			if (goal->GetName()  == orientedName_) {
				goal_ = goal;
				SetName(goal_->GetName());
			}
		}
	}
	// treeを設定する
	if (treeFileName_ != "") {
		tree_->CreateTree(treeFileName_);
	}
}
