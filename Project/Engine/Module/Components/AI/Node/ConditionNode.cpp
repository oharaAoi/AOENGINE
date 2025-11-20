#include "ConditionNode.h"
#include <type_traits>
#include <variant>

ConditionNode::ConditionNode() {
	color_ = ImColor(238, 130, 238);
	baseColor_ = color_;
	type_ = NodeType::Selector;
	SetNodeName("Condition");
}

json ConditionNode::ToJson() {
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

BehaviorStatus ConditionNode::Execute() {
	if (children_.empty()) {return BehaviorStatus::Inactive;}

	BehaviorStatus status;
	if (Compare(worldState_->Get(leftKey_), worldState_->Get(rightKey_), conditionOps[opIndex32_])) {
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
	ImGui::Combo("Operator", &opIndex32_, conditionOps, kOperatorCount_);
	ImGui::RadioButton("Free", &radioButtonIndex_, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Key", &radioButtonIndex_, 1);
	if (radioButtonIndex_ == 0) {
		ImGui::DragFloat("value", &freeValue_);
	} else if (radioButtonIndex_ == 1) {
		worldState_->KeyCombo(rightKey_, rightKeyIndex_, "rightKey");
	}
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

