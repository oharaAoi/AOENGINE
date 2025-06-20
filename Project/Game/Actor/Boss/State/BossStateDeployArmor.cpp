#include "BossStateDeployArmor.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/State/BossIdleState.h"
#include "Engine/Lib/Json/JsonItems.h"

void BossStateDeployArmor::OnStart() {
	SetName("DeployArmor State");
	armorParam_.FromJson(JsonItems::GetData(GetName(), armorParam_.GetName()));
	// armorを取得しておく
	armor_ = pOwner_->GetPulseArmor();
	armor_->SetArmor(armorParam_.durability, armorParam_.scale, armorParam_.color, armorParam_.edgeColor);
	armor_->SetUvSRT(armorParam_.uvScale, armorParam_.uvRotate, armorParam_.uvTranslate);
}

void BossStateDeployArmor::OnUpdate() {
	if (armor_->BreakArmor()) {
		stateMachine_->ChangeState<BossIdleState>();
	}
}

void BossStateDeployArmor::OnExit() {
}

#ifdef _DEBUG
void BossStateDeployArmor::Debug_Gui() {
	ImGui::DragFloat("durability", &armorParam_.durability, 0.1f);
	ImGui::DragFloat3("scale", &armorParam_.scale.x, 0.1f);
	ImGui::ColorEdit4("baseColor", &armorParam_.color.x);
	ImGui::ColorEdit4("edgeColor", &armorParam_.edgeColor.x);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("uvTransform", flags)) {
		if (ImGui::TreeNode("scale")) {
			ImGui::DragFloat3("uvScale", &armorParam_.uvScale.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("rotation")) {
			ImGui::DragFloat3("uvRotation", &armorParam_.uvRotate.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("translation")) {
			ImGui::DragFloat3("uvTranslation", &armorParam_.uvTranslate.x, 0.01f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	
	if (ImGui::Button("Save")) {
		JsonItems::Save(GetName(), armorParam_.ToJson(armorParam_.GetName()));
	}

	armor_->SetArmor(armorParam_.durability, armorParam_.scale, armorParam_.color, armorParam_.edgeColor);
	armor_->SetUvSRT(armorParam_.uvScale, armorParam_.uvRotate, armorParam_.uvTranslate);
}
#endif