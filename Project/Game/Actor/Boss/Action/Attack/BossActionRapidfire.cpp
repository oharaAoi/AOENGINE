#include "BossActionRapidfire.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/Bullet/BossBullet.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/UI/Boss/BossUIs.h"

BehaviorStatus BossActionRapidfire::Execute() {
	return Action();
}

float BossActionRapidfire::EvaluateWeight() {
	return weight_;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionRapidfire::Debug_Gui() {
	BaseTaskNode::Debug_Gui();
	param_.Debug_Gui();
}

void BossActionRapidfire::Parameter::Debug_Gui() {
	ImGui::DragFloat("shotInterval", &shotInterval, .1f);
	ImGui::DragFloat("bulletSpeed", &bulletSpeed, .1f);
	ImGui::DragInt("kFireCount", &kFireCount, 1);
	ImGui::DragFloat("recoveryTime", &recoveryTime, 0.1f);
	ImGui::DragFloat("takeDamage", &takeDamage, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionRapidfire::IsFinish() {
	if (isFinishShot_) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionRapidfire::CanExecute() {
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionRapidfire::Init() {
	param_.SetGroupName("BossAction");
	param_.Load();
	fireCount_ = param_.kFireCount;

	isFinishShot_ = false;
	attackStart_ = false;

	// 警告を出す
	pTarget_->GetUIs()->PopAlert(pTarget_->GetTargetPos(), pTarget_->GetPosition());
	pTarget_->SetIsAttack(false);

	waitTimer_.targetTime_ = param_.recoveryTime;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionRapidfire::Update() {
	if (!attackStart_) {
		attackStart_ = pTarget_->TargetLook();
		return;
	}

	taskTimer_ += GameTimer::DeltaTime();
	if (taskTimer_ > param_.shotInterval) {
		Shot();
		taskTimer_ = 0.0f;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionRapidfire::End() {
	pTarget_->SetIsAttack(true);
}

void BossActionRapidfire::Shot() {
	fireCount_--;

	Vector3 pos = pTarget_->GetPosition();
	Vector3 velocity = (pTarget_->GetTargetPos() - pos).Normalize();

	BossBullet* bullet = pTarget_->GetBulletManager()->AddBullet<BossBullet>(pos, velocity * param_.bulletSpeed);
	bullet->SetTakeDamage(param_.takeDamage);

	if (fireCount_ == 0) {
		isFinishShot_ = true;
	}
}
