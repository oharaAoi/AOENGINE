#include "PlayerManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerManager::Init() {

	// player
	player_ = std::make_unique<Player>();

	// bullet
	bulletManager_ = std::make_unique<PlayerBulletManager>();
	bulletManager_->Init();

	// weapon
	machineGun_ = std::make_unique<MachineGun>();
	machineGun_->Init();

	launcherGun_ = std::make_unique<LauncherGun>();
	launcherGun_->Init();

	shoulderMissile_ = std::make_unique<ShoulderMissile>();
	shoulderMissile_->Init();

	// 初期設定
	player_->SetBulletManager(bulletManager_.get());
	player_->SetWeapon(launcherGun_.get(), LEFT_WEAPON);
	player_->SetWeapon(machineGun_.get(), RIGHT_WEAPON);
	player_->SetWeapon(shoulderMissile_.get(), RIGHT_SHOULDER);

	player_->Init();

	machineGun_->GetTransform()->SetParent(player_->GetRightHandMat());
	machineGun_->SetBulletManager(bulletManager_.get());

	launcherGun_->GetTransform()->SetParent(player_->GetLeftHandMat());
	launcherGun_->SetBulletManager(bulletManager_.get());

	shoulderMissile_->GetTransform()->SetParent(player_->GetRightShoulderMat());
	shoulderMissile_->SetBulletManager(bulletManager_.get());

	armors_ = std::make_unique<Armors>();
	armors_->Init("Player");
	armors_->SetParent(player_->GetTransform()->GetWorldMatrix());
	player_->AddChild(armors_.get());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerManager::Update() {
	CheckAction();

	player_->Update();
	machineGun_->Update();
	launcherGun_->Update();

	bulletManager_->Update(player_->GetTargetPos());

	armors_->Update();
}

void PlayerManager::CheckAction() {
	if (player_->GetIsDeployArmor()) {
		if (!armors_->GetIsDeploy()) {
			armors_->SetArmor();
		}
	}

	if (armors_->BreakArmor()) {
		player_->SetDeployArmor(false);
		armors_->SetIsDeploy(false);
	}
}
