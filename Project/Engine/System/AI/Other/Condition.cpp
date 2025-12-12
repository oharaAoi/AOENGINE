#include "Condition.h"
#include "Engine/System/Manager/ImGuiManager.h"

using json = nlohmann::json;
using namespace AI;

void Condition::Debug_Gui(Blackboard* _state) {
	_state->KeyCombo(leftKey_, leftKeyIndex_, "leftKey");
	ImGui::SetNextItemWidth(80);
	ImGui::Combo("Operator", &opIndex_, conditionOps, kOperatorCount_);
	_state->KeyCombo(rightKey_, rightKeyIndex_, "rightKey");
}

bool Condition::Execute(Blackboard* _state) {
	if (leftKey_ != "" && rightKey_ != "") {
		if (Compare(_state->Get(leftKey_), _state->Get(rightKey_), conditionOps[opIndex_])) {
			return true;
		}
	}
	return false;
}

bool Condition::Compare(const BlackboardValue& lhs,
						const BlackboardValue& rhs,
						const std::string& op) {
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

json Condition::ToJson() {
	json item;
	item["leftKey"] = leftKey_;
	item["leftKeyIndex"] = leftKeyIndex_;
	item["rightKey"] = rightKey_;
	item["rightKeyIndex"] = rightKeyIndex_;
	item["operatorIndex"] = opIndex_;
	return item;
}

void Condition::FromJson(const json& _jsonData) {
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