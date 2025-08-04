#include "WeightSelectorNode.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

WeightSelectorNode::WeightSelectorNode() {
	color_ = ImColor(144, 238, 144);
	baseColor_ = color_;
	type_ = NodeType::WeightSelector;
	SetNodeName("WeightSelector");
}

BehaviorStatus WeightSelectorNode::Execute() {
	if (children_.empty()) {
		currentIndex_ = 0;
		return BehaviorStatus::Inactive;
	}

	// すべてのNodeを実行する
	if (weightMap_.empty()) {
		for (uint32_t index = 0; index < children_.size(); ++index) {
			float weight = children_[index]->EvaluateWeight();
			weightMap_[index] = weight;
		}

		if (weightMap_.empty()) {
			return BehaviorStatus::Success;
		}

		// weightの中から一番値の高いものを取得する
		auto it = std::max_element(
			weightMap_.begin(), weightMap_.end(),
			[](const auto& a, const auto& b) {
				return a.second < b.second;
			}
		);

		currentIndex_ = it->first;
		return BehaviorStatus::Running;
	} else {
		// 選択されたindexを実行する
		BehaviorStatus status = children_[currentIndex_]->Execute();

		if (status == BehaviorStatus::Running) {
			return BehaviorStatus::Running;
		}
		if (status == BehaviorStatus::Failure) {
			currentIndex_ = 0;
			return BehaviorStatus::Failure;
		}
	}
	currentIndex_ = 0;
	return BehaviorStatus::Inactive;
}

void WeightSelectorNode::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());
	InputTextWithString("ReName:", "##wightSelector", node_.name);
}
