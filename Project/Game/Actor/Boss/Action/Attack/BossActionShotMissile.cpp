#include "BossActionShotMissile.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/Bullet/BossMissile.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/UI/Boss/BossUIs.h"

BehaviorStatus BossActionShotMissile::Execute() {
	return Action();
}

float BossActionShotMissile::EvaluateWeight() {
	float result = weight_ * CalcAggressionScore(pTarget_->GetAggressionScore());
	return std::clamp(result, 0.0f, 1.0f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotMissile::Debug_Gui() {
	BaseTaskNode::Debug_Gui();
	param_.Debug_Gui();
}

void BossActionShotMissile::Parameter::Debug_Gui() {
	ImGui::DragFloat("shotInterval", &shotInterval, .1f);
	ImGui::DragFloat("bulletSpeed", &bulletSpeed, .1f);
	SaveAndLoad();
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
	param_.Load();
	taskTimer_ = 0.0f;
	isFinishShot_ = false;
	attackStart_ = false;

	fireCount_ = kFireCount_;

	// 警告を出す
	pTarget_->GetUIs()->PopAlert(pTarget_->GetTargetPos(), pTarget_->GetPosition());
	pTarget_->SetIsAttack(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotMissile::Update() {
	if (!attackStart_) {
		attackStart_ = pTarget_->TargetLook();
		return;
	}

	taskTimer_ += AOENGINE::GameTimer::DeltaTime();
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
	Math::QuaternionSRT srt = pTarget_->GetTransform()->GetSRT();
	// 前方向のベクトルを計算する
	Math::Vector3 pos = srt.translate;
	Math::Vector3 forward = srt.rotate.MakeForward();
	Math::Vector3 up = srt.rotate.MakeUp(); // Y軸に限らず回転軸として使う

	Math::Vector3 velocity = forward.Normalize() * param_.bulletSpeed;
	BossMissile* missile = pTarget_->GetBulletManager()->AddBullet<BossMissile>(pos, velocity, pTarget_->GetTargetPos(),
																				param_.bulletSpeed, param_.firstSpeedRaito, param_.trakingRaito, true);
	missile->SetTakeDamage(param_.takeDamage);

	if (fireCount_ == 0) {
		isFinishShot_ = true;
	}
}