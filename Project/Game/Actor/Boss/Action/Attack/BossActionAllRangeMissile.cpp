#include "BossActionAllRangeMissile.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/Bullet/BossMissile.h"
#include "Game/UI/Boss/BossUIs.h"

BehaviorStatus BossActionAllRangeMissile::Execute() {
	return Action();
}

float BossActionAllRangeMissile::EvaluateWeight() {
	return weight_;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionAllRangeMissile::Debug_Gui() {
	BaseTaskNode::Debug_Gui();
	param_.Debug_Gui();
}

void BossActionAllRangeMissile::Parameter::Debug_Gui() {
	ImGui::DragFloat("recovery", &recoveryTime, 1.0f);
	ImGui::DragFloat("bulletSpeed", &bulletSpeed, 1.0f);
	ImGui::DragFloat("takeDamage", &takeDamage, 1.0f);
	ImGui::DragScalar("fireNum", ImGuiDataType_U32, &fireNum);
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
	playerToRotation_ = Math::Quaternion::LookAt(pTarget_->GetPosition(), pTarget_->GetTargetPos());

	mainAction_ = std::bind(&BossActionAllRangeMissile::LookPlayer, this);

	isFinishShot_ = false;

	// 警告を出す
	pTarget_->GetUIs()->PopAlert(pTarget_->GetTargetPos(), pTarget_->GetPosition());
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
	Math::Vector3 pos = pTarget_->GetTransform()->GetPos();
	Math::Vector3 forward = pTarget_->GetTransform()->GetRotate().MakeForward();
	Math::Vector3 right = pTarget_->GetTransform()->GetRotate().MakeRight();
	Math::Vector3 up = pTarget_->GetTransform()->GetRotate().MakeUp(); // Y軸に限らず回転軸として使う

	const uint32_t angleUpNum = 4;
	const float halfAngle = kPI * 0.5f; // 90度

	// 発射の数文ループする
	for (uint32_t upCount = 0; upCount < angleUpNum; ++upCount) {
		for (uint32_t i = 0; i < param_.fireNum; ++i) {
			// -90度〜+90度に等間隔で弾を発射
			float angle = -halfAngle + (kPI * i) / (param_.fireNum - 1);
			float upAngle = -halfAngle * ((float)upCount / (float)angleUpNum);

			// クォータニオンで回転を生成（up軸周りに角度回転）
			Math::Quaternion rot = Math::Quaternion::AngleAxis(angle, up);
			Math::Quaternion upoRot = Math::Quaternion::AngleAxis(upAngle, right);

			// 正面ベクトルを回転させて発射方向に
			Math::Vector3 dir = upoRot.Rotate(forward);
			dir = rot.Rotate(dir);

			Math::Vector3 velocity = dir.Normalize() * param_.bulletSpeed;
			BossMissile* missile = pTarget_->GetBulletManager()->AddBullet<BossMissile>(pos, velocity, pTarget_->GetTargetPos(),
																						param_.bulletSpeed, param_.firstSpeedRaito, param_.trakingRaito, true);
			missile->SetTakeDamage(param_.takeDamage);
		}
	}

	isFinishShot_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Playerの方向を向かせる処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionAllRangeMissile::LookPlayer() {
	float t = taskTimer_ / lookTime_;
	Math::Quaternion lookRotation = Math::Quaternion::Slerp(pTarget_->GetTransform()->GetRotate(), playerToRotation_, t);
	pTarget_->GetTransform()->SetRotate(lookRotation);

	// 次の行動に遷移する
	if (taskTimer_ > lookTime_) {
		mainAction_ = std::bind(&BossActionAllRangeMissile::Shot, this);
	}
}
