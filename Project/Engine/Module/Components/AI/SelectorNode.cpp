#include "SelectorNode.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

SelectorNode::SelectorNode() {
	color_ = ImColor(144, 238, 144);
	baseColor_ = color_;
	type_ = NodeType::Selector;
	SetNodeName("Selector");
}

BehaviorStatus SelectorNode::Execute() {
	while (currentIndex_ < static_cast<uint32_t>(children_.size())) {
		BehaviorStatus status = children_[currentIndex_]->Execute();

		if (status == BehaviorStatus::Running) {
			return BehaviorStatus::Running;
		}

		if (status == BehaviorStatus::Failure) {
			++currentIndex_;
			return BehaviorStatus::Failure;
		}

		if (status == BehaviorStatus::Success) {
			currentIndex_ = 0;
			return BehaviorStatus::Success;
		}

		// 上記に当てはまらなかったら次のnodeへ
		++currentIndex_;
	}

	// 全て成功
	currentIndex_ = 0;
	return BehaviorStatus::Inactive;
}

float SelectorNode::EvaluateWeight() {
	if (children_.empty()) { return 0; }
	// セレクタの場合は先頭の子どもの重さを計算する
	return children_[0]->EvaluateWeight();
}

void SelectorNode::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());
	InputTextWithString("ReName:","##selector", node_.name);
}
