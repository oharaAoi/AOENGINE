#include "BossActionAllRangeMissile.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/Bullet/BossMissile.h"
#include "Game/UI/Boss/BossUIs.h"
#include "Engine/Lib/Json/JsonItems.h"

BehaviorStatus BossActionAllRangeMissile::Execute() {
	return Action();
}

float BossActionAllRangeMissile::EvaluateWeight() {
	return 0.1f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionAllRangeMissile::Debug_Gui() {
	ITaskNode::Debug_Gui();
	param_.Debug_Gui();
}

void BossActionAllRangeMissile::Parameter::Debug_Gui() {
	ImGui::DragFloat("recovery", &recoveryTime, 1.0f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionAllRangeMissile::IsFinish() {
	if (isFinishShot_) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionAllRangeMissile::CanExecute() {
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionAllRangeMissile::Init() {
	param_.SetGroupName("BossAction");
	param_.Load();
	// 
	taskTimer_ = 0.f;
	playerToRotation_ = Quaternion::LookAt(pTarget_->GetPosition(), pTarget_->GetPlayerPosition());

	mainAction_ = std::bind(&BossActionAllRangeMissile::LookPlayer, this);

	isFinishShot_ = false;

	bulletSpeed_ = 80.f;

	// 警告を出す
	pTarget_->GetUIs()->PopAlert(pTarget_->GetPlayerPosition(), pTarget_->GetPosition());
	pTarget_->SetIsAttack(false);

	waitTimer_.targetTime_ = param_.recoveryTime;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionAllRangeMissile::Update() {
	taskTimer_ += GameTimer::DeltaTime();
	mainAction_();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionAllRangeMissile::End() {
	pTarget_->SetIsAttack(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 弾を撃つ処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionAllRangeMissile::Shot() {
	if (isFinishShot_) { return; }
	Vector3 pos = pTarget_->GetTransform()->GetPos();
	Vector3 forward = pTarget_->GetTransform()->GetRotate().MakeForward();
	Vector3 right = pTarget_->GetTransform()->GetRotate().MakeRight();
	Vector3 up = pTarget_->GetTransform()->GetRotate().MakeUp(); // Y軸に限らず回転軸として使う

	const uint32_t angleUpNum = 4;
	const uint32_t fireNum = 9;
	const float halfAngle = kPI * 0.5f; // 90度

	for (uint32_t upCount = 0; upCount < angleUpNum; ++upCount) {
		for (uint32_t i = 0; i < fireNum; ++i) {
			// -90度〜+90度に等間隔で弾を発射
			float angle = -halfAngle + (kPI * i) / (fireNum - 1);
			float upAngle = -halfAngle * ((float)upCount / (float)angleUpNum);

			// クォータニオンで回転を生成（up軸周りに角度回転）
			Quaternion rot = Quaternion::AngleAxis(angle, up);
			Quaternion upoRot = Quaternion::AngleAxis(upAngle, right);

			// 正面ベクトルを回転させて発射方向に
			Vector3 dir = upoRot.Rotate(forward);
			dir = rot.Rotate(dir);

			Vector3 velocity = dir.Normalize() * bulletSpeed_;
			BossMissile* missile = pTarget_->GetBulletManager()->AddBullet<BossMissile>(pos, velocity, pTarget_->GetPlayerPosition(), bulletSpeed_,0.1f, 0.05f, true);
			missile->SetTakeDamage(30.0f);
		}
	}

	isFinishShot_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Playerの方向を向かせる処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionAllRangeMissile::LookPlayer() {
	float t = taskTimer_ / lookTime_;
	Quaternion lookRotation = Quaternion::Slerp(pTarget_->GetTransform()->GetRotate(), playerToRotation_, t);
	pTarget_->GetTransform()->SetRotate(lookRotation);

	// 次の行動に遷移する
	if (taskTimer_ > lookTime_) {
		mainAction_ = std::bind(&BossActionAllRangeMissile::Shot, this);
	}
}
