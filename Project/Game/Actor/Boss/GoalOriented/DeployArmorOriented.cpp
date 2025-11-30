#include "DeployArmorOriented.h"
#include "Engine/System/Manager/ImGuiManager.h"

DeployArmorOriented::DeployArmorOriented() {
	SetName("DeployArmor");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 評価値の計算
///////////////////////////////////////////////////////////////////////////////////////////////

float DeployArmorOriented::CalculationScore() {
	if (worldState_->Get("deployArmor").As<bool>()) {
		return 1.0f;
	}
	return 0.0f;
}

void DeployArmorOriented::Debug_Gui() {
	ImGui::DragInt("priority", &priority_, 1);
}
