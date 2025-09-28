#include "BossStateDeployArmor.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/State/BossIdleState.h"
#include "Engine/Lib/Json/JsonItems.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateDeployArmor::OnStart() {
	SetName("DeployArmorState");

	armorParam_.SetGroupName(GetName());
	armorParam_.Load();
	// armorを取得しておく
	armor_ = pOwner_->GetPulseArmor();
	armor_->SetArmor();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateDeployArmor::OnUpdate() {
	if (armor_->BreakArmor()) {
		stateMachine_->ChangeState<BossIdleState>();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateDeployArmor::OnExit() {
	pOwner_->SetIsArmorDeploy(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateDeployArmor::Debug_Gui() {
	armorParam_.Debug_Gui();
}

void BossStateDeployArmor::ArmorParameter::Debug_Gui() {
	ImGui::DragFloat("durability", &durability, 0.1f);
	ImGui::DragFloat3("scale", &scale.x, 0.1f);
	ImGui::ColorEdit4("baseColor", &color.x);
	ImGui::ColorEdit4("edgeColor", &edgeColor.x);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("uvTransform", flags)) {
		if (ImGui::TreeNode("scale")) {
			ImGui::DragFloat3("uvScale", &uvTransform.scale.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("rotation")) {
			ImGui::DragFloat3("uvRotation", &uvTransform.rotate.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("translation")) {
			ImGui::DragFloat3("uvTranslation", &uvTransform.translate.x, 0.01f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	SaveAndLoad();
}