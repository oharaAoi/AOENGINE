#include "BossStateDeployArmor.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/State/BossIdleState.h"
#include "Engine/Lib/Json/JsonItems.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateDeployArmor::OnStart() {
	SetName("DeployArmorState");
	armorParam_.FromJson(JsonItems::GetData(GetName(), armorParam_.GetName()));
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
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateDeployArmor::Debug_Gui() {
	ImGui::DragFloat("durability", &armorParam_.durability, 0.1f);
	ImGui::DragFloat3("scale", &armorParam_.scale.x, 0.1f);
	ImGui::ColorEdit4("baseColor", &armorParam_.color.x);
	ImGui::ColorEdit4("edgeColor", &armorParam_.edgeColor.x);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("uvTransform", flags)) {
		if (ImGui::TreeNode("scale")) {
			ImGui::DragFloat3("uvScale", &armorParam_.uvTransform.scale.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("rotation")) {
			ImGui::DragFloat3("uvRotation", &armorParam_.uvTransform.rotate.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("translation")) {
			ImGui::DragFloat3("uvTranslation", &armorParam_.uvTransform.translate.x, 0.01f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	
	if (ImGui::Button("Save")) {
		JsonItems::Save(GetName(), armorParam_.ToJson(armorParam_.GetName()));
	}
}