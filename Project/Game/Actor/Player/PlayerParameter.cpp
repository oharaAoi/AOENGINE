#include "PlayerParameter.h"
#include "Engine/System/Manager/ImGuiManager.h"

void PlayerParameter::Debug_Gui() {
	ImGui::DragFloat("health", &health, 0.1f);
	ImGui::DragFloat("postureStability", &postureStability, 0.1f);
	ImGui::DragFloat("姿勢安定回復時間", &psRecoveryTime, 0.1f);
	ImGui::DragFloat("姿勢安定回復量", &psRecoveryValue, 0.1f);
	ImGui::DragFloat("bodyWeight", &bodyWeight, 0.1f);
	ImGui::DragFloat("energy", &energy, 0.1f);
	ImGui::DragFloat("energyRecoveryAmount", &energyRecoveryAmount, 0.1f);
	ImGui::DragFloat("energyRecoveryCoolTime", &energyRecoveryCoolTime, 0.1f);

	ImGui::DragFloat("legColliderRadius", &legColliderRadius, 0.1f);
	ImGui::DragFloat("legColliderPosY", &legColliderPosY, 0.1f);

	ImGui::DragFloat("windDrag", &windDrag, 0.1f);

	ImGui::DragFloat("inclineStrength", &inclineStrength, 0.01f, 0.0f);
	ImGui::DragFloat("inclineReactionRate", &inclineReactionRate, 0.01f, 0.0f);
	ImGui::DragFloat("inclineThreshold", &inclineThreshold, 0.01f, 0.0f);
	ImGui::DragFloat3("cameraOffset", &cameraOffset.x, 0.01f, 0.0f);

	ImGui::DragFloat3("translateOffset", &translateOffset.x, 0.01f, 0.0f);

	ImGui::DragFloat("ピンチの割合", &pinchOfPercentage, 0.01f, 0.0f);
	ImGui::DragFloat("ビネットの強さ", &pinchVignettePower, 0.01f, 0.0f);
	ImGui::ColorEdit4("ビネットの色", &pinchVignetteColor.r);

	ImGui::DragFloat("無敵時間", &invincibleTime, 0.1f);

	SaveAndLoad();
}