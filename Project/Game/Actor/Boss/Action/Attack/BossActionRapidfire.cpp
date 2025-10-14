#include "BossActionRapidfire.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/Bullet/BossBullet.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/UI/Boss/BossUIs.h"

BehaviorStatus BossActionRapidfire::Execute() {
	return Action();
}

float BossActionRapidfire::EvaluateWeight() {
	return 0.9f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionRapidfire::Debug_Gui() {
	ITaskNode::Debug_Gui();
	param_.Debug_Gui();
}

void BossActionRapidfire::Parameter::Debug_Gui() {
	ImGui::DragFloat("shotInterval", &shotInterval, .1f);
	ImGui::DragFloat("bulletSpeed", &bulletSpeed, .1f);
	ImGui::DragInt("kFireCount", &kFireCount, 1);
	ImGui::DragFloat("coolTime", &coolTime, 0.1f);
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

	// 警告を出す
	pTarget_->GetUIs()->PopAlert(pTarget_->GetPlayerPosition(), pTarget_->GetPosition());
	pTarget_->SetIsAttack(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionRapidfire::Update() {
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
	coolTime_ = param_.coolTime;
	pTarget_->SetIsAttack(true);
}

void BossActionRapidfire::Shot() {
	fireCount_--;

	Vector3 pos = pTarget_->GetPosition();
	Vector3 velocity = (pTarget_->GetPlayerPosition() - pos).Normalize();

	BossBullet* bullet = pTarget_->GetBulletManager()->AddBullet<BossBullet>(pos, velocity * param_.bulletSpeed);
	bullet->SetTakeDamage(10.0f);

	if (fireCount_ == 0) {
		isFinishShot_ = true;
	}
}
