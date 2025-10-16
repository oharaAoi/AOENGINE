#include "BossActionShotgun.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/Bullet/BossBullet.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/UI/Boss/BossUIs.h"

BehaviorStatus BossActionShotgun::Execute() {
	return Action();
}

float BossActionShotgun::EvaluateWeight() {
	return 0.7f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotgun::Debug_Gui() {
	ITaskNode::Debug_Gui();
	param_.Debug_Gui();
}

void BossActionShotgun::Parameter::Debug_Gui() {
	ImGui::DragFloat("bulletSpeed", &bulletSpeed, .1f);
	ImGui::DragFloat("stiffenTime", &bulletSpread, .1f);
	ImGui::DragInt("kFireCount", &kFireCount, 1);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionShotgun::IsFinish() {
	if (isFinishShot_) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionShotgun::CanExecute() {
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotgun::Init() {
	param_.SetGroupName("BossAction");
	param_.Load();
	
	taskTimer_ = 0.0f;
	isFinishShot_ = false;
	attackStart_ = false;

	// 警告を出す
	pTarget_->GetUIs()->PopAlert(pTarget_->GetPlayerPosition(), pTarget_->GetPosition());
	pTarget_->SetIsAttack(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotgun::Update() {
	if (!attackStart_) {
		attackStart_ = pTarget_->TargetLook();
		return;
	}

	Shot();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotgun::End() {
	pTarget_->SetIsAttack(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ user function
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotgun::Shot() {
	Vector3 pos = pTarget_->GetPosition();
	Vector3 velocity = (pTarget_->GetPlayerPosition() - pos).Normalize();
	// ばらつきを弧度法に
	float bulletSpread = param_.bulletSpread * kToRadian;

	// 弾数分処理をする
	for (int oi = 0; oi < param_.kFireCount; ++oi) {
		float yawOffset = RandomFloat(-bulletSpread, bulletSpread);
		float pitchOffset = RandomFloat(-bulletSpread, bulletSpread);

		Quaternion yawRot = Quaternion::AngleAxis(yawOffset, CVector3::UP);
		Quaternion pitchRot = Quaternion::AngleAxis(pitchOffset, CVector3::RIGHT);
		Quaternion spreadRot = yawRot * pitchRot;

		Vector3 dir = spreadRot * velocity;
		BossBullet* bullet = pTarget_->GetBulletManager()->AddBullet<BossBullet>(pos, dir * param_.bulletSpeed);
		bullet->SetTakeDamage(10.0f);
	}

	isFinishShot_ = true;
}