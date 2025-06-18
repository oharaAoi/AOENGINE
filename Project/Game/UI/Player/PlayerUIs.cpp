#include "PlayerUIs.h"
#include "Engine/Lib/Json/JsonItems.h"

void PlayerUIs::Init(Player* _player) {
	SetName("PlayerUIs");
	pPlayer_ = _player;

	health_ = std::make_unique<BaseGaugeUI>();
	health_->Init("gauge_bg.png", "gauge_front.png");

	postureStability_ = std::make_unique<PostureStability>();
	postureStability_->Init();

	uiItems_.FromJson(JsonItems::GetData(GetName(), uiItems_.GetName()));

	health_->SetScale(uiItems_.healthScale);
	health_->SetCenterPos(uiItems_.healthPos);

	postureStability_->SetScale(uiItems_.postureScale);
	postureStability_->SetCenterPos(uiItems_.posturePos);
}

void PlayerUIs::Update() {
	const Player::Parameter& playerParam = pPlayer_->GetParam();
	const Player::Parameter& playerInitParam = pPlayer_->GetInitParam();

	health_->SetFillAmount(playerParam.health / playerInitParam.health);
	health_->Update();
	postureStability_->Update(playerParam.postureStability / playerInitParam.postureStability);
}

void PlayerUIs::Draw() const {
	health_->Draw();
	postureStability_->Draw();
 }

#ifdef _DEBUG
void PlayerUIs::Debug_Gui() {
	if (ImGui::CollapsingHeader("Health")) {
		ImGui::DragFloat2("HealthScale", &uiItems_.healthScale.x, 0.1f);
		ImGui::DragFloat2("HealthPos", &uiItems_.healthPos.x, 0.1f);

		health_->SetScale(uiItems_.healthScale);
		health_->SetCenterPos(uiItems_.healthPos);
	}

	if (ImGui::CollapsingHeader("PostureStability")) {
		ImGui::DragFloat2("postureScale", &uiItems_.postureScale.x, 0.1f);
		ImGui::DragFloat2("posturePos", &uiItems_.posturePos.x, 0.1f);

		postureStability_->SetScale(uiItems_.postureScale);
		postureStability_->SetCenterPos(uiItems_.posturePos);
	}


	if (ImGui::Button("Save")) {
		JsonItems::Save(GetName(), uiItems_.ToJson(uiItems_.GetName()));
	}
}
#endif