#include "BossLotteryAction.h"
#include "imgui.h"
#include "Engine/Lib/Json/JsonItems.h"

void BossLotteryAction::Init(const std::string& actionName) {
	weightParameter_.SetName(actionName);
	weightParameter_.FromJson(JsonItems::GetData("BossActionWeight", weightParameter_.GetName()));

	initWeightParameter_ = weightParameter_;
}

void BossLotteryAction::Debug_Gui() {

	if (ImGui::TreeNode("LotteryWeight")) {

		ImGui::DragFloat("weight", &weightParameter_.actionWeight_.weight);
		ImGui::DragFloat("weightInc", &weightParameter_.actionWeight_.weightInc);

		if (ImGui::Button("Save")) {
			JsonItems::Save("BossActionWeight", weightParameter_.ToJson(weightParameter_.GetName()));
		}
		if (ImGui::Button("Apply")) {
			weightParameter_.FromJson(JsonItems::GetData("BossActionWeight", weightParameter_.GetName()));
		}
		ImGui::TreePop();
	}
}

void BossLotteryAction::Reset() {
	weightParameter_ = initWeightParameter_;
}

void BossLotteryAction::Increase() {
	weightParameter_.actionWeight_.weight += weightParameter_.actionWeight_.weightInc;
}

