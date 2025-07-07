#include "SequenceNode.h"

SequenceNode::SequenceNode() {
	color_ = ImVec4(147 / 255, 112 / 255, 219 / 255, 1.f);
	SetName("Sequence");
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
	return BehaviorStatus::Success;
}