#include "BossActionVerticalMissile.h"
#include "Game/Actor/Boss/Boss.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Actor/Boss/Bullet/BossMissile.h"

BehaviorStatus BossActionVerticalMissile::Execute() {
	return Action();
}

float BossActionVerticalMissile::EvaluateWeight() {
	return 0.2f;
}
///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionVerticalMissile::Debug_Gui() {
	ITaskNode::Debug_Gui();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionVerticalMissile::IsFinish() {
	if (param_.kFireCount <= fireCount_) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行可能確認
//////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionVerticalMissile::CanExecute() {
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionVerticalMissile::Init() {
	param_.FromJson(JsonItems::GetData("BossAction", param_.GetName()));

	taskTimer_ = 0.0f;
	fireCount_ = 0;

	pTarget_->SetIsAttack(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionVerticalMissile::Update() {
	taskTimer_ += GameTimer::DeltaTime();
	if (taskTimer_ > 0.2f) {
		Shot();
		taskTimer_ = 0.0f;
	} 
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionVerticalMissile::End() {
	pTarget_->SetIsAttack(true);
}

void BossActionVerticalMissile::Shot() {
	Vector3 forward = pTarget_->GetTransform()->GetRotate().MakeForward();
	Vector3 right = pTarget_->GetTransform()->GetRotate().MakeRight();
	Vector3 up = pTarget_->GetTransform()->GetRotate().MakeUp(); // Y軸に限らず回転軸として使う

	const float dx[2] = {2, -2};
	
	angle_ = 90.0f - (static_cast<float>(fireCount_) * 10.0f);
	angle_ *= -kToRadian;
	// クォータニオンで回転を生成（up軸周りに角度回転）
	Quaternion rot = Quaternion::AngleAxis(angle_, right);
	
	// 正面ベクトルを回転させて発射方向に
	Vector3 dir = rot.Rotate(forward);
	
	for (uint32_t i = 0; i < 2; i++) {
		Vector3 pos = pTarget_->GetTransform()->GetPos() + (param_.fireRadius * dir);
		pos += right * dx[i];
		Vector3 velocity = dir.Normalize() * param_.bulletSpeed;
		BossMissile* missile = pTarget_->GetBulletManager()->AddBullet<BossMissile>(pos, velocity, pTarget_->GetPlayerPosition(), param_.bulletSpeed, 0.05f, true);
		missile->SetTakeDamage(30.0f);
	}

	fireCount_++;
}