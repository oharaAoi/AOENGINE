#include "ConditionNode.h"
#include <type_traits>
#include <variant>

ConditionNode::ConditionNode() {
	color_ = ImColor(238, 130, 238);
	baseColor_ = color_;
	type_ = NodeType::Condition;
	SetNodeName("Condition");
}

json ConditionNode::ToJson() {
	json item;
	item["name"] = node_.name;
	item["nodeType"] = static_cast<int>(type_);
	item["nodePos"] = json{ {"x", pos_.x}, {"y", pos_.y} };
	item["children"] = json::array();
	// 特有の項目
	item["leftKey"] = leftKey_;
	item["leftKeyIndex"] = leftKeyIndex_;
	item["rightKey"] = rightKey_;
	item["rightKeyIndex"] = rightKeyIndex_;
	item["operatorIndex"] = opIndex_;

	for (const auto& child : children_) {
		item["children"].push_back(child->ToJson());
	}
	return item;
}

void ConditionNode::FromJson(const json& _jsonData) {
	node_.name = _jsonData["name"];
	type_ = _jsonData["nodeType"];
	pos_ = Vector2(_jsonData["nodePos"]["x"], _jsonData["nodePos"]["y"]);
	if (_jsonData.contains("leftKey")) {
		leftKey_ = _jsonData["leftKey"];
	}

	if (_jsonData.contains("leftKeyIndex")) {
		leftKeyIndex_ = _jsonData["leftKeyIndex"];
	}

	if (_jsonData.contains("rightKey")) {
		rightKey_ = _jsonData["rightKey"];
	}

	if (_jsonData.contains("rightKeyIndex")) {
		rightKeyIndex_ = _jsonData["rightKeyIndex"];
	}

	if (_jsonData.contains("operatorIndex")) {
		opIndex_ = _jsonData["operatorIndex"];
	}
}

BehaviorStatus ConditionNode::Execute() {
	if (children_.empty()) {return BehaviorStatus::Inactive;}

	BehaviorStatus status;
	if (Compare(worldState_->Get(leftKey_), worldState_->Get(rightKey_), conditionOps[opIndex_])) {
		status = children_[0]->Execute();
	} else {
		status = children_[1]->Execute();
	}

	if (status == BehaviorStatus::Running) {
		return BehaviorStatus::Running;
	}

	if (status == BehaviorStatus::Failure) {
		return BehaviorStatus::Failure;
	}

	if (status == BehaviorStatus::Success) {
		return BehaviorStatus::Success;
	}

	return BehaviorStatus::Inactive;
}

float ConditionNode::EvaluateWeight() {
	if (children_.empty()) { return 0; }
	// セレクタの場合は先頭の子どもの重さを計算する
	return children_[0]->EvaluateWeight();
}

void ConditionNode::Debug_Gui() {
	worldState_->KeyCombo(leftKey_, leftKeyIndex_, "leftKey");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	ImGui::Combo("Operator", &opIndex_, conditionOps, kOperatorCount_);
	ImGui::SameLine();
	worldState_->KeyCombo(rightKey_, rightKeyIndex_, "rightKey");
}

bool ConditionNode::Compare(const WorldStateValue& lhs, const WorldStateValue& rhs, const std::string& op) {
	return std::visit([&](auto&& a, auto&& b) -> bool {
		using A = std::decay_t<decltype(a)>;
		using B = std::decay_t<decltype(b)>;

		// 異なる型は比較できない
		if constexpr (!std::is_same_v<A, B>) {
			return false;
		} else {
			if (op == "==") return a == b;
			if (op == "!=") return a != b;
			if (op == ">")  return a > b;
			if (op == "<")  return a < b;
			if (op == ">=") return a >= b;
			if (op == "<=") return a <= b;
			return false;
		}
					  }, lhs.Get(), rhs.Get());
}

