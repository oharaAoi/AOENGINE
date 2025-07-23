#include "PlayerUIs.h"
#include "Engine/Lib/Json/JsonItems.h"

void PlayerUIs::Init(Player* _player) {
	SetName("PlayerUIs");
	pPlayer_ = _player;

	ap_ = Engine::CreateSprite("AP.png");

	health_ = std::make_unique<BaseGaugeUI>();
	health_->Init("gauge_bg.png", "gauge_front.png");

	postureStability_ = std::make_unique<PostureStability>();
	postureStability_->Init();

	uiItems_.FromJson(JsonItems::GetData(GetName(), uiItems_.GetName()));

	ap_->SetScale(uiItems_.apScale);
	ap_->SetTranslate(uiItems_.apPos);

	health_->SetScale(uiItems_.healthScale);
	health_->SetCenterPos(uiItems_.healthPos);

	postureStability_->SetScale(uiItems_.postureScale);
	postureStability_->SetCenterPos(uiItems_.posturePos);

	Engine::GetCanvas2d()->AddSprite(ap_.get());
}

void PlayerUIs::Update() {
	const Player::Parameter& playerParam = pPlayer_->GetParam();
	const Player::Parameter& playerInitParam = pPlayer_->GetInitParam();

	ap_->Update();

	health_->SetFillAmount(playerParam.health / playerInitParam.health);
	health_->Update();
	postureStability_->Update(playerParam.postureStability / playerInitParam.postureStability);
}

void PlayerUIs::Draw() const {

 }

void PlayerUIs::Debug_Gui() {
	if (ImGui::CollapsingHeader("AP")) {
		ImGui::DragFloat2("apScale", &uiItems_.apScale.x, 0.1f);
		ImGui::DragFloat2("apPos", &uiItems_.apPos.x, 0.1f);

		ap_->SetScale(uiItems_.apScale);
		ap_->SetTranslate(uiItems_.apPos);
	}

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