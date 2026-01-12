#include "ParallelNode.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

using namespace AI;

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コンストラクタ
///////////////////////////////////////////////////////////////////////////////////////////////

AI::ParallelNode::ParallelNode() {
    color_ = ImColor(144, 238, 144);
    baseColor_ = color_;
    type_ = NodeType::Parallel;
	successCount_ = 0;
    SetNodeName("Parallel");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ jsonに変換
///////////////////////////////////////////////////////////////////////////////////////////////

json AI::ParallelNode::ToJson() {
	json item;
	item["name"] = node_.name;
	item["nodeType"] = static_cast<int>(type_);
	item["nodePos"] = json{ {"x", pos_.x}, {"y", pos_.y} };
	item["children"] = json::array();

	for (const auto& child : children_) {
		item["children"].push_back(child->ToJson());
	}
	return item;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行
///////////////////////////////////////////////////////////////////////////////////////////////

BehaviorStatus AI::ParallelNode::Execute() {
	BehaviorStatus status = BehaviorStatus::Failure;
	// 子のNodeをすべて同時に実行する
	for (auto& child : children_) {
		if (child->GetState() != BehaviorStatus::Success) {
			status = child->Execute();
		}

		if (status == BehaviorStatus::Running) {
			status = BehaviorStatus::Running;
		}

		if (status == BehaviorStatus::Failure) {
			status =  BehaviorStatus::Failure;
		}

		if (status == BehaviorStatus::Success) {
			successCount_++;
		}
	}

	if (successCount_ == children_.size()) {
		return BehaviorStatus::Success;
	}
	
	return status;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 評価値の計算
///////////////////////////////////////////////////////////////////////////////////////////////

float AI::ParallelNode::EvaluateWeight() {
	float weight = 0.0f;
	for (const auto& child : children_) {
		weight += child->EvaluateWeight();
	}
    return weight / static_cast<float>(children_.size());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AI::ParallelNode::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());
	InputTextWithString("ReName:", "##parallel", node_.name);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行中の名前を返す
///////////////////////////////////////////////////////////////////////////////////////////////

std::string AI::ParallelNode::RunNodeName() {
	if (children_.empty()) {
		return this->GetName();
	} else {
		return children_[0]->RunNodeName();
	}
}
