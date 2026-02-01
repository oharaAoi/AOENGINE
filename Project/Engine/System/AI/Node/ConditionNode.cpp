#include "ConditionNode.h"
#include <type_traits>
#include <variant>

using namespace AI;

ConditionNode::ConditionNode() {
	color_ = ImColor(238, 130, 238);
	baseColor_ = color_;
	type_ = NodeType::Condition;
	preIndex_ = 0;
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
	pos_ = Math::Vector2(_jsonData["nodePos"]["x"], _jsonData["nodePos"]["y"]);
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
	if (children_.empty()) { return BehaviorStatus::Inactive; }

	// 並び変えを行なう
	std::sort(children_.begin(), children_.end(),
			  [](BaseBehaviorNode* a, BaseBehaviorNode* b) {
				  return a->GetPos().x < b->GetPos().x; // X座標が小さい順
			  });

	BehaviorStatus status;
	if (Compare(blackboard_->Get(leftKey_), blackboard_->Get(rightKey_), conditionOps[opIndex_])) {
		currentIndex_ = 0;
		status = children_[0]->Execute();
	} else {
		// 右のNodeが設定されていたら
		if (children_.size() >= 2) {
			currentIndex_ = 1;
			status = children_[1]->Execute();
		} else {
			// 設定されていなかったら失敗を返す
			currentIndex_ = 0;
			return BehaviorStatus::Failure;
		}
	}

	/*if (currentIndex_ != preIndex_) {
		if (!children_.empty()) {
			if (preIndex_ == 0) {
				children_[preIndex_]->ResetNode();
			} else if (children_.size() >= 2) {
				children_[preIndex_]->ResetNode();
			}
		}
	}*/

	preIndex_ = currentIndex_;

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
	if (!blackboard_) return;

	blackboard_->KeyCombo(leftKey_, leftKeyIndex_, "leftKey");
	ImGui::SetNextItemWidth(80);
	ImGui::Combo("Operator", &opIndex_, conditionOps, kOperatorCount_);
	blackboard_->KeyCombo(rightKey_, rightKeyIndex_, "rightKey");

	const BlackboardValue& lhs = blackboard_->Get(leftKey_);
	const BlackboardValue& rhs = blackboard_->Get(rightKey_);

	std::visit([&](auto&& a, auto&& b) {
		using A = std::decay_t<decltype(a)>;
		using B = std::decay_t<decltype(b)>;

		using BaseA = ValueTypeT<A>;
		using BaseB = ValueTypeT<B>;

		if constexpr (!std::is_same_v<BaseA, BaseB>) {
			ImGui::Text("２つの型が異なるため比較できません");
		} else if constexpr (!std::is_arithmetic_v<BaseA> && !std::is_same_v<BaseA, std::string>) {
			return ImGui::Text("比較演算子が定義されていません");
		}
			   }, lhs.Get(), rhs.Get());
}

std::string ConditionNode::RunNodeName() {
	return BaseRunNodeName();
}

void AI::ConditionNode::ResetNode() {
	if (children_.empty()) { return; }
	children_[currentIndex_]->ResetNode();
}

bool ConditionNode::Compare(const BlackboardValue& lhs, const BlackboardValue& rhs, const std::string& op) {
	return std::visit([&](auto&& a, auto&& b) -> bool {
		using A = std::decay_t<decltype(a)>;
		using B = std::decay_t<decltype(b)>;

		using BaseA = ValueTypeT<A>;
		using BaseB = ValueTypeT<B>;

		// 基底型が違う場合は比較不可
		if constexpr (!std::is_same_v<BaseA, BaseB>) {
			return false;
		}
		// 比較演算が定義されていない型も弾く
		else if constexpr (!std::is_arithmetic_v<BaseA> && !std::is_same_v<BaseA, std::string>) {
			return false;
		} else {
			const BaseA& va = GetValue(a);
			const BaseA& vb = GetValue(b);

			if (op == "==") return va == vb;
			if (op == "!=") return va != vb;
			if (op == ">")  return va > vb;
			if (op == "<")  return va < vb;
			if (op == ">=") return va >= vb;
			if (op == "<=") return va <= vb;
			return false;
		}
					  }, lhs.Get(), rhs.Get());
}

