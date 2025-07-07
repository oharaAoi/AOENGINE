#include "SelectorNode.h"

SelectorNode::SelectorNode() {
	color_ = ImVec4(147 / 255, 112 / 255, 219 / 255, 1.f);
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
