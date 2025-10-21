#include "WeightSelectorNode.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/Lib/Math/MyRandom.h"

WeightSelectorNode::WeightSelectorNode() {
	color_ = ImColor(144, 238, 144);
	baseColor_ = color_;
	type_ = NodeType::WeightSelector;
	SetNodeName("WeightSelector");
	isReset_ = false;
}

json WeightSelectorNode::ToJson() {
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

BehaviorStatus WeightSelectorNode::Execute() {
	if (children_.empty()) {
		currentIndex_ = 0;
		return BehaviorStatus::Inactive;
	}

	// すべてのNodeを実行する
	if (weightMap_.empty() || isReset_) {
		for (uint32_t index = 0; index < children_.size(); ++index) {
			float weight = children_[index]->EvaluateWeight();
			weightMap_[index] = weight;
		}

		if (weightMap_.empty()) {
			return BehaviorStatus::Success;
		}

		std::vector<uint32_t> keys;
		std::vector<double> weights;
		for (auto& [key, value] : weightMap_) {
			keys.push_back(key);
			weights.push_back(static_cast<double>(value));
		}

		// 重みに基づく乱択
		std::random_device rd;
		std::mt19937 gen(rd());
		std::discrete_distribution<> dist(weights.begin(), weights.end());

		currentIndex_ = keys[dist(gen)];
		children_[currentIndex_]->Execute();
		isReset_ = false;
		return BehaviorStatus::Running;
	} else {
		// 選択されたindexを実行する
		BehaviorStatus status = children_[currentIndex_]->Execute();

		if (status == BehaviorStatus::Success) {
			isReset_ = true;
			return BehaviorStatus::Success;
		}
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
