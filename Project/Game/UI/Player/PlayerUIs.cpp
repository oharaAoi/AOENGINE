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
	rightShoulderWeapon_ = std::make_unique<WeaponRemainingRounds>();
	leftWeapon_->Init("leftWeaponGauge");
	rightWeapon_->Init("rightWeaponGauge");
	rightShoulderWeapon_->Init("rightShoulderWeaponGauge");

	AddChild(ap_);
	AddChild(health_.get());
	AddChild(energyOutput_.get());
	AddChild(postureStability_.get());
	AddChild(leftWeapon_.get());
	AddChild(rightWeapon_.get());
	AddChild(rightShoulderWeapon_.get());

	EditorWindows::AddObjectWindow(this, "PlayerUIs");
}

void PlayerUIs::Update(const Vector2& reticlePos) {
	const Player::Parameter& playerParam = pPlayer_->GetParam();
	const Player::Parameter& playerInitParam = pPlayer_->GetInitParam();

	ap_->Update();

	energyOutput_->Update(playerParam.energy / playerInitParam.energy);

	health_->Update(playerParam.health / playerInitParam.health);

	if (pPlayer_->GetIsDeployArmor()) {
		postureStability_->SetGaugeType(GaugeType::Armor);
		postureStability_->Update(1.0f - (playerParam.postureStability / playerInitParam.postureStability));
	} else {
		postureStability_->SetGaugeType(GaugeType::Posturebility);
		postureStability_->Update(playerParam.postureStability / playerInitParam.postureStability);
	}

	// ---------------------------
	// ↓ weaponの残弾数ゲージを更新
	// ---------------------------

	BaseWeapon* left = pPlayer_->GetWeapon(PlayerWeapon::LEFT_WEAPON);
	BaseWeapon* right = pPlayer_->GetWeapon(PlayerWeapon::RIGHT_WEAPON);
	BaseWeapon* rightShoulder = pPlayer_->GetWeapon(PlayerWeapon::RIGHT_SHOULDER);

	// 左手武器の残弾数
	if (left->GetIsReload()) {
		leftWeapon_->Update(reticlePos, pPlayer_->GetWeapon(PlayerWeapon::LEFT_WEAPON)->ReloadFill());
		leftWeapon_->Blinking();
	} else {
		leftWeapon_->Update(reticlePos, pPlayer_->GetWeapon(PlayerWeapon::LEFT_WEAPON)->BulletsFill());
	}

	// 右手武器の残弾数
	if (right->GetIsReload()) {
		rightWeapon_->Update(reticlePos, pPlayer_->GetWeapon(PlayerWeapon::RIGHT_WEAPON)->ReloadFill());
		rightWeapon_->Blinking();
	} else {
		rightWeapon_->Update(reticlePos, pPlayer_->GetWeapon(PlayerWeapon::RIGHT_WEAPON)->BulletsFill());
	}

	// 右肩武器の残弾数
	if (rightShoulder->GetIsReload()) {
		rightShoulderWeapon_->Update(reticlePos, pPlayer_->GetWeapon(PlayerWeapon::RIGHT_SHOULDER)->ReloadFill());
		rightShoulderWeapon_->Blinking();
	} else {
		rightShoulderWeapon_->Update(reticlePos, pPlayer_->GetWeapon(PlayerWeapon::RIGHT_SHOULDER)->BulletsFill());
	}
}

void PlayerUIs::Debug_Gui() {
	
}