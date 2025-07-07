#include "BehaviorRootNode.h"
#include <utility>

BehaviorRootNode::BehaviorRootNode() {
	color_ = ImVec4(0, 0, 205 / 255, 1.0f);
	SetNodeName("Root");
}

BehaviorStatus BehaviorRootNode::Execute() {
	if (children_.size() > 1) {
		children_.erase(children_.begin());
	}

	while (currentIndex_ < static_cast<uint32_t>(children_.size())) {
		BehaviorStatus status = children_[currentIndex_]->Execute();

		if (status == BehaviorStatus::Running)
			return BehaviorStatus::Running;

		if (status == BehaviorStatus::Failure || BehaviorStatus::Success) {
			++currentIndex_;
			continue;
		}

		// 成功したので次へ
		++currentIndex_;
	}

	// 全て成功
	currentIndex_ = 0;
	return BehaviorStatus::Success;
}

