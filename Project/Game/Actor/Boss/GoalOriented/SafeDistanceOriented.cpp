#include "SafeDistanceOriented.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/AI/State/Blackboard.h"

SafeDistanceOriented::SafeDistanceOriented() {
    SetName("SafeDistance");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 評価値の計算
///////////////////////////////////////////////////////////////////////////////////////////////

float SafeDistanceOriented::CalculationScore() {
	float distance = blackboard_->Get("BossToPlayer").As<float>();
	float dangerDistance = blackboard_->Get("dangerDistance").As<float>();

	if (distance < dangerDistance) {
		return 1.0f;
	}
	return 0.4f;
} 

void SafeDistanceOriented::Debug_Gui() {
    ImGui::DragInt("priority", &priority_, 1);
}
