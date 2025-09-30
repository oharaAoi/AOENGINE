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

	uiItems_.Load();

	ap_->SetScale(uiItems_.apScale);
	ap_->SetTranslate(uiItems_.apPos);

	health_->SetScale(uiItems_.healthScale);
	health_->SetCenterPos(uiItems_.healthPos);

	postureStability_->SetScale(uiItems_.postureScale);
	postureStability_->SetCenterPos(uiItems_.posturePos);

	leftWeapon_ = std::make_unique<WeaponRemainingRounds>();
	rightWeapon_ = std::make_unique<WeaponRemainingRounds>();
	leftWeapon_->Init("leftWeaponGauge");
	rightWeapon_->Init("rightWeaponGauge");

	AddChild(leftWeapon_.get());
	AddChild(rightWeapon_.get());

	Engine::GetCanvas2d()->AddSprite(ap_.get());

	EditorWindows::AddObjectWindow(this, "PlayerUIs");
}

void PlayerUIs::Update(const Vector2& reticlePos) {
	const Player::Parameter& playerParam = pPlayer_->GetParam();
	const Player::Parameter& playerInitParam = pPlayer_->GetInitParam();

	ap_->Update();

	health_->SetFillAmount(playerParam.health / playerInitParam.health);
	health_->Update();
	postureStability_->Update(playerParam.postureStability / playerInitParam.postureStability);

	leftWeapon_->Update(reticlePos, pPlayer_->GetWeapon(PlayerWeapon::LEFT_WEAPON)->BulletsFill());
	rightWeapon_->Update(reticlePos, pPlayer_->GetWeapon(PlayerWeapon::RIGHT_WEAPON)->BulletsFill());
}

void PlayerUIs::Draw() const {

 }

void PlayerUIs::Debug_Gui() {
	uiItems_.Debug_Gui();

	ap_->SetScale(uiItems_.apScale);
	ap_->SetTranslate(uiItems_.apPos);
	health_->SetScale(uiItems_.healthScale);
	health_->SetCenterPos(uiItems_.healthPos);
	postureStability_->SetScale(uiItems_.postureScale);
	postureStability_->SetCenterPos(uiItems_.posturePos);
}

void PlayerUIs::UIItems::Debug_Gui() {
	ImGui::DragFloat2("apScale", &apScale.x, 0.1f);
	ImGui::DragFloat2("apPos", &apPos.x, 0.1f);
	ImGui::DragFloat2("HealthScale", &healthScale.x, 0.1f);
	ImGui::DragFloat2("HealthPos", &healthPos.x, 0.1f);
	ImGui::DragFloat2("postureScale", &postureScale.x, 0.1f);
	ImGui::DragFloat2("posturePos", &posturePos.x, 0.1f);
	SaveAndLoad();
}