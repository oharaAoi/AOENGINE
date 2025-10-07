#include "PlayerUIs.h"
#include "Engine/Lib/Json/JsonItems.h"

void PlayerUIs::Init(Player* _player) {
	SetName("PlayerUIs");
	pPlayer_ = _player;

	Canvas2d* canvas = Engine::GetCanvas2d();
	ap_ = canvas->AddSprite("AP.png", "ap");
	ap_->SetName("AP");
	ap_->Load("PlayerUIs", "AP");

	energyOutput_ = std::make_unique<EnergyOutput>();
	energyOutput_->Init("PlayerUIs", "EnergyOutput");

	health_ = std::make_unique<Health>();
	health_->Init("PlayerUIs", "Health");

	postureStability_ = std::make_unique<PostureStability>();
	postureStability_->Init("PlayerUIs", "PostureStability");

	leftWeapon_ = std::make_unique<WeaponRemainingRounds>();
	rightWeapon_ = std::make_unique<WeaponRemainingRounds>();
	leftWeapon_->Init("leftWeaponGauge");
	rightWeapon_->Init("rightWeaponGauge");

	AddChild(ap_);
	AddChild(health_.get());
	AddChild(energyOutput_.get());
	AddChild(postureStability_.get());
	AddChild(leftWeapon_.get());
	AddChild(rightWeapon_.get());

	EditorWindows::AddObjectWindow(this, "PlayerUIs");
}

void PlayerUIs::Update(const Vector2& reticlePos) {
	const Player::Parameter& playerParam = pPlayer_->GetParam();
	const Player::Parameter& playerInitParam = pPlayer_->GetInitParam();

	ap_->Update();

	energyOutput_->Update(playerParam.energy / playerInitParam.energy);

	health_->Update(playerParam.health / playerInitParam.health);
	postureStability_->Update(playerParam.postureStability / playerInitParam.postureStability);

	// ---------------------------
	// ↓ weaponの残弾数ゲージを更新
	// ---------------------------

	BaseWeapon* left = pPlayer_->GetWeapon(PlayerWeapon::LEFT_WEAPON);
	BaseWeapon* right = pPlayer_->GetWeapon(PlayerWeapon::RIGHT_WEAPON);

	if (left->GetIsReload()) {
		leftWeapon_->Update(reticlePos, pPlayer_->GetWeapon(PlayerWeapon::LEFT_WEAPON)->ReloadFill());
		leftWeapon_->Blinking();
	} else {
		leftWeapon_->Update(reticlePos, pPlayer_->GetWeapon(PlayerWeapon::LEFT_WEAPON)->BulletsFill());
	}

	if (right->GetIsReload()) {
		rightWeapon_->Update(reticlePos, pPlayer_->GetWeapon(PlayerWeapon::RIGHT_WEAPON)->ReloadFill());
		rightWeapon_->Blinking();
	} else {
		rightWeapon_->Update(reticlePos, pPlayer_->GetWeapon(PlayerWeapon::RIGHT_WEAPON)->BulletsFill());
	}
}

void PlayerUIs::Debug_Gui() {
	
}