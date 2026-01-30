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
	weapons_[PlayerWeapon::Left_Weapon] = std::make_unique<LauncherGun>();
	weapons_[PlayerWeapon::Right_Weapon] = std::make_unique<MachineGun>();
	weapons_[PlayerWeapon::Right_Shoulder] = std::make_unique<ShoulderMissile>();
	weapons_[PlayerWeapon::Left_Shoulder] = std::make_unique<LaserRifle>();

	for (auto& [type, weapon] : weapons_) {
		weapon->Init();
		weapon->SetBulletManager(bulletManager_.get());
		weapon->GetTransform()->SetParent(player_->GetWeaponBornMatrix(type));
		player_->SetWeapon(weapon.get(), type);
	}

	// 初期設定
	player_->SetBulletManager(bulletManager_.get());
	player_->Init();

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
	bulletManager_->Update(player_->GetTargetPos());
	armors_->Update();
}

void PlayerManager::PostUpdate() {
	player_->UpdateJoint();
	for (auto& weapon : weapons_) {
		weapon.second->Update();
	}
	player_->PosUpdate();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ playerの死亡確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerManager::CheckIsDead() {
	return player_->GetIsExplode();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ actionのチェック
///////////////////////////////////////////////////////////////////////////////////////////////

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
