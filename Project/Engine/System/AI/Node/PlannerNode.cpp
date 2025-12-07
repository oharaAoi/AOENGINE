#include "PlannerNode.h"
#include "Engine/System/AI/BehaviorTreeSystem.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

namespace fs = std::filesystem;
using namespace AI;

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コンストラクタ
///////////////////////////////////////////////////////////////////////////////////////////////

PlannerNode::PlannerNode(const std::unordered_map<std::string, std::shared_ptr<BaseBehaviorNode>>& _rootNodeCanTask,
						 Blackboard* _worldState,
						 const std::vector<std::shared_ptr<IOrientedGoal>>& _goals) {
	canTask_ = _rootNodeCanTask;
	pBlackboard_ = _worldState;
	goalArray_ = _goals;

	// 所有する
	tree_ = BehaviorTreeSystem::GetInstance()->Create();
	tree_->Init();
	tree_->SetCanTaskMap(_rootNodeCanTask);
	tree_->SetBlackboard(pBlackboard_);
	tree_->SetName("plannerTree");

	color_ = ImColor(255, 99, 71);
	baseColor_ = color_;
	type_ = NodeType::Planner;
	SetNodeName("Planner");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ jsonに変換
///////////////////////////////////////////////////////////////////////////////////////////////

json PlannerNode::ToJson() {
	if (goal_) {
		orientedName_ = goal_->GetName();
	}

	json item;
	item["name"] = node_.name;
	item["nodeType"] = static_cast<int>(type_);
	item["nodePos"] = json{ {"x", pos_.x}, {"y", pos_.y} };
	item["children"] = json::array();
	item["orientedName"] = orientedName_;
	item["treeFileName"] = treeFileName_;
	item["goalCondition"] = condition_.ToJson();

	for (const auto& child : children_) {
		item["children"].push_back(child->ToJson());
	}
	return item;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ jsonから作成
///////////////////////////////////////////////////////////////////////////////////////////////

void PlannerNode::FromJson(const json& _jsonData) {
	node_.name = _jsonData["name"];
	type_ = _jsonData["nodeType"];
	pos_ = Math::Vector2(_jsonData["nodePos"]["x"], _jsonData["nodePos"]["y"]);
	if (_jsonData.contains("orientedName")) {
		orientedName_ = _jsonData["orientedName"];
	}

	if (_jsonData.contains("treeFileName")) {
		treeFileName_ = _jsonData["treeFileName"];
	}

	if (_jsonData.contains("goalCondition")) {
		condition_.FromJson(_jsonData["goalCondition"]);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行
///////////////////////////////////////////////////////////////////////////////////////////////

BehaviorStatus PlannerNode::Execute() {
	bool isExecute = tree_->Run();
	if (condition_.Execute(pBlackboard_)) {
		return BehaviorStatus::Success;
	}

	if (isExecute) {
		return BehaviorStatus::Running;
	} else {
		return BehaviorStatus::Failure;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ weight値を計算
///////////////////////////////////////////////////////////////////////////////////////////////

float PlannerNode::EvaluateWeight() {
	if (goal_) {
		goal_->Update();
		return goal_->CalculationScore();
	}
	return 0.0f;
}

std::string PlannerNode::RunNodeName() {
	return tree_->GetRootNode()->GetCurrentRunNodeName();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlannerNode::Debug_Gui() {
	EditNodeName();

	// Treeの編集
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
	condition_.Debug_Gui(pBlackboard_);
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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 目標の設定
///////////////////////////////////////////////////////////////////////////////////////////////

void PlannerNode::SetGOBT(const std::string _orientedName, const std::string _treeFileName) {
	orientedName_ = _orientedName;
	treeFileName_ = _treeFileName;
	// goalを設定する
	if (orientedName_ != "") {
		for (auto goal : goalArray_) {
			if (goal->GetName() == orientedName_) {
				goal_ = goal;
				SetName(goal_->GetName());
			}
		}
	}
	// treeを設定する
	if (treeFileName_ != "") {
		std::filesystem::path p(treeFileName_);
		std::string fileName = p.filename().string();
		tree_->SetName(fileName);
		tree_->CreateTree(treeFileName_);
	}
}
