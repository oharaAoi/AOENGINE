#include "ShoulderMissile.h"
#include "Engine/System/Audio/AudioPlayer.h"
#include "Game/Actor/Weapon/Bullet/RocketBullet.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShoulderMissile::Finalize() {

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShoulderMissile::Debug_Gui() {
	if (ImGui::CollapsingHeader("Base")) {
		BaseWeapon::Debug_Gui();
	}

	if (ImGui::CollapsingHeader("Unique")) {
		weaponParam_.Debug_Gui();
	}

	transform_->SetTranslate(weaponParam_.pos);
}

void ShoulderMissile::ShoulderMissileParam::Debug_Gui() {
	ImGui::DragFloat3("pos", &pos.x, 0.1f);
	ImGui::DragFloat("trackingLength", &trackingLength, 0.1f);
	ImGui::DragFloat("trackingTime", &trackingTime, 0.1f);
	ImGui::DragFloat("trackingRaito", &trackingRaito, 0.1f);
	ImGui::DragFloat("shotSeVolum", &shotSeVolum, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShoulderMissile::Init() {
	SetName("ShoulderMissile");
	BaseWeapon::Init();
	attackParam_.SetName("shoulderMissileAttackParam");
	attackParam_.Load();
	weaponParam_.Load();

	object_->SetObject("shoulderMissile.obj");

	transform_->SetTranslate(weaponParam_.pos);

	isFinish_ = true;
	isReload_ = false;
	coolTime_ = 5;

	EditorWindows::AddObjectWindow(this, GetName());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShoulderMissile::Update() {
	transform_->Update();
	if (isFinish_) { return; }

	if (isReload_) {
		coolTime_ -= GameTimer::DeltaTime();
		if (coolTime_ < 0.0f) {
			isReload_ = false;
			isFinish_ = true;
			isCanAttack_ = true;
			fireCount_ = 0;
		}
		return;
	}

	if (coolTime_ > 0) {
		coolTime_ -= GameTimer::DeltaTime();
	} else {
		Shot();

		if (fireCount_ >= attackParam_.fireBulletsNum) {
			coolTime_ = attackParam_.reloadTime;
			isReload_ = true;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 攻撃処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool ShoulderMissile::Attack(const AttackContext& cxt) {
	if (!isCanAttack_) { return false; }

	isCanAttack_ = false;
	attackCxt_ = cxt;
	isFinish_ = false;
	Shot();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 発射処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShoulderMissile::Shot() {
	Vector3 worldPos = object_->GetPosition();
	RocketBullet* bullet = pBulletManager_->AddBullet<RocketBullet>(worldPos, attackCxt_.target, attackParam_.bulletSpeed,
																	weaponParam_.trackingLength, weaponParam_.trackingTime, weaponParam_.trackingRaito);
	bullet->SetTakeDamage(attackParam_.takeDamage);

	coolTime_ = attackParam_.fireInterval;
	fireCount_++;

	// 音を鳴らす
	AudioPlayer::SinglShotPlay("missileShot.mp3", weaponParam_.shotSeVolum);
}