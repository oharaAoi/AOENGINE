#include "PlayerManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerManager::Init() {

	player_ = std::make_unique<Player>();
	player_->Init();

	bulletManager_ = std::make_unique<PlayerBulletManager>();
	bulletManager_->Init();

	machineGun_ = std::make_unique<MachineGun>();
	machineGun_->Init();

	launcherGun_ = std::make_unique<LauncherGun>();
	launcherGun_->Init();

	player_->SetBulletManager(bulletManager_.get());
	player_->SetWeapon(launcherGun_.get(), LEFT_WEAPON);
	player_->SetWeapon(machineGun_.get(), RIGHT_WEAPON);

	machineGun_->GetTransform()->SetParent(player_->GetRightHandMat());
	machineGun_->SetBulletManager(bulletManager_.get());

	launcherGun_->GetTransform()->SetParent(player_->GetLeftHandMat());
	launcherGun_->SetBulletManager(bulletManager_.get());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerManager::Update() {
	player_->Update();

	bulletManager_->Update();

	machineGun_->Update();
	launcherGun_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerManager::Draw() const {
	Engine::SetPipeline(PSOType::Object3d, "Object_Normal.json");
	player_->Draw();

	bulletManager_->Draw();

	machineGun_->Draw();
	launcherGun_->Draw();
}
