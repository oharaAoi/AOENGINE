#include "BehaviorRootNode.h"
#include <utility>

BehaviorRootNode::BehaviorRootNode() {
	color_ = ImColor(0, 191, 255);
	baseColor_ = color_;
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

void BehaviorRootNode::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());
}

