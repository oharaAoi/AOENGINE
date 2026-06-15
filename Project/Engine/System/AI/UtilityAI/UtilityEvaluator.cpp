#include "UtilityEvaluator.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

using namespace AI;

float UtilityEvaluator::Evaluate(float _inputValue) const {
	float t = std::clamp(_inputValue / maxValue, 0.0f, 1.0f);
	float curveValue = curve.BezierValue(t);
	return curveValue * weight;
}

void UtilityEvaluator::Debug_Gui() {
	if (ImGui::CollapsingHeader("Evaluator")) {
		InputTextWithString("Group", "##Utility AI Group", groupName_);
		InputTextWithString("Key", "##Utility AI Key", name_);
		ImGui::DragFloat("Weight", &weight);
		ImGui::DragFloat("Max Value", &maxValue);
		curve.Debug_Gui();

		SetName(name_);
		SetGroupName(groupName_);

		SaveAndLoad();
	}
}

void UtilityEvaluator::LoadJson(const std::string& _group, const std::string& _key) {
	SetGroupName(_group);
	SetName(_key);
	Load();
}