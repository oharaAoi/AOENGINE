#include "SelectorNode.h"

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
