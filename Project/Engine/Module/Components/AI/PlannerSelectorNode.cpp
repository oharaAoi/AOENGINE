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

		// key と weight の配列を作る
		std::vector<uint32_t> keys;
		std::vector<double> weights;
		for (auto& [key, value] : priorityMap_) {
			keys.push_back(key);
			weights.push_back(static_cast<double>(std::clamp(value, 0.0f, 1.0f)));
		}

		// 重みに基づく乱択
		std::random_device rd;
		std::mt19937 gen(rd());
		std::discrete_distribution<> dist(weights.begin(), weights.end());

		currentIndex_ = keys[dist(gen)];
		children_[currentIndex_]->Execute();
		children_[currentIndex_]->SetState(BehaviorStatus::Running);
		reset_ = false;

#ifdef _DEBUG
		PriorityDisplay();
#endif

		return BehaviorStatus::Running;
	} else {
		// 選択されたindexを実行する
		BehaviorStatus status = children_[currentIndex_]->Execute();

#ifdef _DEBUG
		PriorityDisplay();
#endif

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

void PlannerSelectorNode::PriorityDisplay() {
	std::vector<std::pair<uint32_t, float>> priorityArray(priorityMap_.begin(), priorityMap_.end());

	// 値を降順でソート
	std::sort(priorityArray.begin(), priorityArray.end(), [](const auto& a, const auto& b) {
		return a.second > b.second;
			  });

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking;

	if (ImGui::Begin("PlannerSelector Priority Window", nullptr, flags)) {
		for (uint32_t index = 0; index < priorityArray.size(); ++index) {
			ImGui::Text(children_[priorityArray[index].first]->GetName().c_str());
			ImGui::Text("priority : %f", priorityArray[index].second);
			ImGui::Separator();
		}
	}
	ImGui::End();
}
