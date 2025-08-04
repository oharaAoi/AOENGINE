#include "SequenceNode.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

SequenceNode::SequenceNode() {
	color_ = ImColor(144, 238, 144);
	baseColor_ = color_;
	type_ = NodeType::Sequencer;
	SetNodeName("Sequence");
}

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
	}
	// 全て成功
	currentIndex_ = 0;
	return BehaviorStatus::Inactive;
}

float SequenceNode::EvaluateWeight() {
	if (children_.empty()) { return 0; }
	// セレクタの場合は先頭の子どもの重さを計算する
	return children_[0]->EvaluateWeight();
}

void SequenceNode::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());
	InputTextWithString("ReName:", "##sequence", node_.name);
}
