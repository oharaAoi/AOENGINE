#include "BehaviorRootNode.h"
#include <utility>

BehaviorRootNode::BehaviorRootNode() {
	color_ = ImColor(0, 191, 255);
	baseColor_ = color_;
	type_ = NodeType::Root;
	SetNodeName("Root");
}

json BehaviorRootNode::ToJson() {
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

BehaviorStatus BehaviorRootNode::Execute() {
	if (children_.size() > 1) {
		children_.erase(children_.begin());
	}

	while (currentIndex_ < static_cast<uint32_t>(children_.size())) {
		BehaviorStatus status = children_[currentIndex_]->Execute();

		if (status == BehaviorStatus::Running)
			return BehaviorStatus::Running;

		if (status == BehaviorStatus::Success) {
			currentIndex_ = 0;
			return BehaviorStatus::Success;
		}

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

