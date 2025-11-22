#include "SequenceNode.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コンストラクタ
///////////////////////////////////////////////////////////////////////////////////////////////

SequenceNode::SequenceNode() {
	color_ = ImColor(144, 238, 144);
	baseColor_ = color_;
	type_ = NodeType::Sequencer;
	SetNodeName("Sequence");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ json形式へ変換
///////////////////////////////////////////////////////////////////////////////////////////////

json SequenceNode::ToJson() {
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

BehaviorStatus SequenceNode::Execute() {
	while (currentIndex_ < static_cast<uint32_t>(children_.size())) {
		BehaviorStatus status = children_[currentIndex_]->Execute();

		if (status == BehaviorStatus::Running)
			return BehaviorStatus::Running;

		if (status == BehaviorStatus::Failure) {
			currentIndex_ = 0;
			return BehaviorStatus::Failure;
		}

		// 成功したので次へ
		++currentIndex_;

		if (currentIndex_ >= static_cast<uint32_t>(children_.size())) {
			currentIndex_ = 0;
			return BehaviorStatus::Success;
		}
	}
	// 全て成功
	currentIndex_ = 0;
	return BehaviorStatus::Inactive;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 評価値の計算
///////////////////////////////////////////////////////////////////////////////////////////////

float SequenceNode::EvaluateWeight() {
	if (children_.empty()) { return 0; }
	float value = 0;
	for (auto child : children_) {
		value += child->EvaluateWeight();
	}
	value /= static_cast<float>(children_.size());
	return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SequenceNode::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());
	InputTextWithString("ReName:", "##sequence", node_.name);
}
