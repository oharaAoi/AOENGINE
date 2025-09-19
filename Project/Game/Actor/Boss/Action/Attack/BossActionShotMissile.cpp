#include "BossActionShotMissile.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/Bullet/BossMissile.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/UI/Boss/BossUIs.h"

BehaviorStatus BossActionShotMissile::Execute() {
	return Action();
}

float BossActionShotMissile::EvaluateWeight() {
	return 0.4f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotMissile::Debug_Gui() {
	ITaskNode::Debug_Gui();
	ImGui::DragFloat("shotInterval", &param_.shotInterval, .1f);
	ImGui::DragFloat("bulletSpeed", &param_.bulletSpeed, .1f);
	if (ImGui::Button("Save")) {
		JsonItems::Save("BossAction", param_.ToJson(param_.GetName()));
	}
	if (ImGui::Button("Apply")) {
		param_.FromJson(JsonItems::GetData("BossAction", param_.GetName()));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionShotMissile::IsFinish() {
	if (isFinishShot_) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionShotMissile::CanExecute() {
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotMissile::Init() {
	param_.FromJson(JsonItems::GetData("BossAction", param_.GetName()));
	taskTimer_ = 0.0f;
	isFinishShot_ = false;

	fireCount_ = kFireCount_;

	// 警告を出す
	pTarget_->GetUIs()->PopAlert();
	pTarget_->SetIsAttack(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotMissile::Update() {
	taskTimer_ += GameTimer::DeltaTime();

	if (taskTimer_ > param_.shotInterval) {
		Shot();
		taskTimer_ = 0.0f;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotMissile::End() {
	pTarget_->SetIsAttack(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 弾を打つ処理をする
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotMissile::Shot() {
	fireCount_--;

	Vector3 pos = pTarget_->GetTransform()->srt_.translate;
	Vector3 forward = pTarget_->GetTransform()->srt_.rotate.MakeForward();
	Vector3 up = pTarget_->GetTransform()->srt_.rotate.MakeUp(); // Y軸に限らず回転軸として使う

	Vector3 velocity = forward.Normalize() * param_.bulletSpeed;
	BossMissile* missile = pTarget_->GetBulletManager()->AddBullet<BossMissile>(pos, velocity, pTarget_->GetPlayerPosition(), param_.bulletSpeed, 0.5f, true);
	missile->SetTakeDamage(20.0f);

	if (fireCount_ == 0) {
		isFinishShot_ = true;
	}
}