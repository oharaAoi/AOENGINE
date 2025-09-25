#include "PlannerSelectorNode.h"
#include "Engine/Lib/Math/MyRandom.h"

PlannerSelectorNode::PlannerSelectorNode() {
	color_ = ImColor(255, 31, 31);
	baseColor_ = color_;
	type_ = NodeType::PlannerSelector;
	SetNodeName("PlannerSelector");
	reset_ = false;
}

json PlannerSelectorNode::ToJson() {
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

BehaviorStatus PlannerSelectorNode::Execute() {
	if (children_.empty()) {
		currentIndex_ = 0;
		return BehaviorStatus::Inactive;
	}

	// すべてのNodeを実行する
	if (priorityMap_.empty() || reset_) {
		for (uint32_t index = 0; index < children_.size(); ++index) {
			children_[index]->SetState(BehaviorStatus::Inactive);
			float weight = children_[index]->EvaluateWeight();
			priorityMap_[index] = std::clamp(weight, 0.0f, 1.0f);
		}

		if (priorityMap_.empty()) {
			return BehaviorStatus::Success;
		}

		//// weightの中から一番値の高いものを取得する
		//priorityMap_.begin(), priorityMap_.end(),
		//	[](const auto& a, const auto& b) {
		//	return a.second < b.second;
		//	};
		  // key と weight の配列を作る
		std::vector<uint32_t> keys;
		std::vector<double> weights;
		for (auto& [key, value] : priorityMap_) {
			keys.push_back(key);
			weights.push_back(static_cast<double>(value));
		}

		// 重みに基づく乱択
		std::random_device rd;
		std::mt19937 gen(rd());
		std::discrete_distribution<> dist(weights.begin(), weights.end());


 		currentIndex_ = keys[dist(gen)];
		children_[currentIndex_]->Execute();
		children_[currentIndex_]->SetState(BehaviorStatus::Running);
		reset_ = false;
		return BehaviorStatus::Running;
	} else {
		// 選択されたindexを実行する
		BehaviorStatus status = children_[currentIndex_]->Execute();

		if (status == BehaviorStatus::Success) {
			reset_ = true;
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

float PlannerSelectorNode::EvaluateWeight() {
	return 0.0f;
}

void PlannerSelectorNode::Debug_Gui() {
}
