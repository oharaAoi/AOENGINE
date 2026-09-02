#include "PlayerParameter.h"

#include "Engine/System/Manager/ImGuiManager.h"

void PlayerParameter::Debug_Gui() {
	ImGui::SeparatorText("Player Move Parameter");
	ImGui::Spacing();

	ImGui::DragFloat("Move Speed", &moveSpeed, 0.1f);
	ImGui::DragFloat("Move Input Threshold", &moveInputThreshold, 0.001f);

	ImGui::Spacing();
	ImGui::SeparatorText("Player Jump Parameter");
	ImGui::Spacing();

	ImGui::DragFloat("Jump Power", &jumpPower, 0.1f);
	ImGui::DragFloat("Hang Time", &hangTime, 0.01f);
	ImGui::DragFloat("Rise Gravity", &riseGravity, 0.1f);
	ImGui::DragFloat("Fall Gravity", &fallGravity, 0.1f);
	ImGui::DragFloat("Max Fall Speed", &maxFallSpeed, 0.1f);
	ImGui::DragFloat("Ground Keep Speed", &groundKeepSpeed, 0.1f);

	ImGui::Spacing();
	ImGui::SeparatorText("Player Input Parameter");
	ImGui::Spacing();

	ImGui::DragFloat("Stick DeadZone", &stickDeadZone, 0.01f);
	
	ImGui::Spacing();
	ImGui::SeparatorText("Player ConnectState Parameter");
	ImGui::Spacing();

	ImGui::DragFloat("Connectable Time",&connectableTime,0.1f);
	ImGui::DragFloat("Launch Wait Time", &launchWaitTime, 0.1f);
	ImGui::DragFloat("Gather Speed", &gatherSpeed, 0.1f);
	ImGui::DragFloat("Launch Speed", &launchSpeed, 0.1f);
	ImGui::DragFloat("Launch Accel", &launchAccel, 0.1f);
	ImGui::DragFloat("Launch Life Time", &launchLifeTime, 0.1f);

	ImGui::Spacing();
	ImGui::SeparatorText("Player Miscellaneous Parameter");
	ImGui::Spacing();

	ImGui::DragFloat("Ground Height",&groundHeight,0.1f);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Save / Load ボタン
	SaveAndLoad();
}
