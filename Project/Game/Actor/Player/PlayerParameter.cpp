#include "PlayerParameter.h"

#include "Engine/System/Manager/ImGuiManager.h"

void PlayerParameter::Debug_Gui() {
	ImGui::DragFloat("Move Speed", &moveSpeed, 0.1f);
	ImGui::DragFloat("Move Input Threshold", &moveInputThreshold, 0.001f);

	ImGui::DragFloat("Jump Power", &jumpPower, 0.1f);
	ImGui::DragFloat("Hang Time", &hangTime, 0.01f);
	ImGui::DragFloat("Rise Gravity", &riseGravity, 0.1f);
	ImGui::DragFloat("Fall Gravity", &fallGravity, 0.1f);
	ImGui::DragFloat("Max Fall Speed", &maxFallSpeed, 0.1f);

	ImGui::DragFloat("Ground Height", &groundHeight, 0.1f);
	ImGui::DragFloat("Stick DeadZone", &stickDeadZone, 0.01f);

	// Save / Load ボタン
	SaveAndLoad();
}
