#include "WeightSelectorNode.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/Lib/Math/MyRandom.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コンストラクタ
///////////////////////////////////////////////////////////////////////////////////////////////

WeightSelectorNode::WeightSelectorNode() {
	color_ = ImColor(144, 238, 144);
	baseColor_ = color_;
	type_ = NodeType::WeightSelector;
	SetNodeName("WeightSelector");
	isReset_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ jsonに変換
///////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行処理
///////////////////////////////////////////////////////////////////////////////////////////////

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

		// 選択されたnodeを実行
		currentIndex_ = keys[dist(gen)];
		children_[currentIndex_]->Execute();
		isReset_ = false;

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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void WeightSelectorNode::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());
	InputTextWithString("ReName:", "##wightSelector", node_.name);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 評価値の計算
///////////////////////////////////////////////////////////////////////////////////////////////

void WeightSelectorNode::PriorityDisplay() {
	std::vector<std::pair<uint32_t, float>> priorityArray(weightMap_.begin(), weightMap_.end());

	// 値を降順でソート
	std::sort(priorityArray.begin(), priorityArray.end(), [](const auto& a, const auto& b) {
		return a.second > b.second;
			  });

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking;

	// weightの表示
	if (ImGui::Begin("WeightSelector Priority Window", nullptr, flags)) {
		for (uint32_t index = 0; index < priorityArray.size(); ++index) {
			ImGui::Text(children_[priorityArray[index].first]->NodeNameCombination().c_str());
			ImGui::Text("priority : %f", priorityArray[index].second);
			ImGui::Separator();
		}
	}
	ImGui::End();
}