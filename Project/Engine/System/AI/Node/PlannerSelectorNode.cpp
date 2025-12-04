#include "PlannerSelectorNode.h"
#include "Engine/Lib/Math/MyRandom.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コンストラクタ
///////////////////////////////////////////////////////////////////////////////////////////////

PlannerSelectorNode::PlannerSelectorNode() {
	color_ = ImColor(255, 31, 31);
	baseColor_ = color_;
	type_ = NodeType::PlannerSelector;
	SetNodeName("PlannerSelector");
	reset_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ jsonに変換
///////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行
///////////////////////////////////////////////////////////////////////////////////////////////

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

		// 選択された子の実行
		currentIndex_ = keys[dist(gen)];
		children_[currentIndex_]->Execute();
		children_[currentIndex_]->SetState(BehaviorStatus::Running);
		reset_ = false;

#ifdef _DEBUG
		// 評価値の表示
		PriorityDisplay();
#endif

		PriorityOutput();

		return BehaviorStatus::Running;
	} else {
		// 選択されたindexを実行する
		BehaviorStatus status = children_[currentIndex_]->Execute();

#ifdef _DEBUG
		// 評価値の表示
		PriorityDisplay();
#endif

		// 成功したら
		if (status == BehaviorStatus::Success) {
			reset_ = true;
			return BehaviorStatus::Success;
		}
		// 実行中なら
		if (status == BehaviorStatus::Running) {
			return BehaviorStatus::Running;
		}
		// 失敗したら
		if (status == BehaviorStatus::Failure) {
			currentIndex_ = 0;
			return BehaviorStatus::Failure;
		}
	}
	currentIndex_ = 0;
	return BehaviorStatus::Inactive;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 評価値の計算
///////////////////////////////////////////////////////////////////////////////////////////////

float PlannerSelectorNode::EvaluateWeight() {
	return 0.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlannerSelectorNode::Debug_Gui() {}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行中の名前を返す
///////////////////////////////////////////////////////////////////////////////////////////////

std::string PlannerSelectorNode::RunNodeName() {
	if (children_.empty()) {
		return this->GetName();
	} else {
		return children_[currentIndex_]->RunNodeName();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 評価値の表示
///////////////////////////////////////////////////////////////////////////////////////////////

void PlannerSelectorNode::PriorityDisplay() {
	std::vector<std::pair<uint32_t, float>> priorityArray(priorityMap_.begin(), priorityMap_.end());

	// 値を降順でソート
	std::sort(priorityArray.begin(), priorityArray.end(), [](const auto& a, const auto& b) {
		return a.second > b.second;
			  });

	if (ImGui::Begin("PlannerSelector Priority Window", nullptr)) {
		for (uint32_t index = 0; index < priorityArray.size(); ++index) {
			std::string priorityText = "priority : " + std::to_string(priorityArray[index].second);
			std::string nodeName = "[" + children_[priorityArray[index].first]->GetName() + "]";

			ImGui::Text(nodeName.c_str());
			ImGui::Text(priorityText.c_str());
			ImGui::Separator();
		}
	}
	ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 評価値の出力
///////////////////////////////////////////////////////////////////////////////////////////////

void PlannerSelectorNode::PriorityOutput() {
	std::vector<std::pair<uint32_t, float>> priorityArray(priorityMap_.begin(), priorityMap_.end());

	// 値を降順でソート
	std::sort(priorityArray.begin(), priorityArray.end(), [](const auto& a, const auto& b) {
		return a.second > b.second;
			  });

	// weightの表示
	//for (uint32_t index = 0; index < priorityArray.size(); ++index) {
	//	std::string priorityText = "priority : " + std::to_string(priorityArray[index].second);
	//	std::string nodeName = "[" + children_[priorityArray[index].first]->NodeNameCombination() + "] - ";

	//	// logに出力
	//	pLogger_->Log(nodeName.c_str());
	//	pLogger_->Log(priorityText);
	//	pLogger_->Log("\n");
	//}

	//pLogger_->Log("----------------------------------------------\n");
}
