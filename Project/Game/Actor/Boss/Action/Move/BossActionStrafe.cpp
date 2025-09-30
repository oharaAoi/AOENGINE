#include "BossActionStrafe.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Actor/Boss/Boss.h"
#include "Engine/Lib/Math/MyRandom.h"

BehaviorStatus BossActionStrafe::Execute() {
	return Action();
}

float BossActionStrafe::EvaluateWeight() {
	return pTarget_->GetEvaluationFormula()->StrafeEvaluation(10.0f, 50.0f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionStrafe::Debug_Gui() {
	ITaskNode::Debug_Gui();
	param_.Debug_Gui();
}

void BossActionStrafe::Parameter::Debug_Gui() {
	ImGui::DragFloat("moveSpeed", &moveSpeed, 0.1f);
	ImGui::DragFloat("moveTime", &moveTime, 0.1f);
	ImGui::DragFloat("getDistance", &getDistance, 0.1f);
	ImGui::DragFloat("decayRate", &decayRate, 0.1f);
	curve.Debug_Gui();
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionStrafe::IsFinish() {
	if (stopping_) {
		if (velocity_.Length() <= 1.0f) {
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionStrafe::CanExecute() {
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionStrafe::Init() {
	param_.SetGroupName("BossAction");
	param_.Load();
	taskTimer_ = 0;
	stopping_ = false;
	velocity_ = CVector3::ZERO;

	Vector3 bossPosition = pTarget_->GetPosition();
	Vector3 playerPosition = pTarget_->GetPlayerPosition();

	// 距離を計算する
	Vector3 distance = bossPosition - playerPosition;
	distance.y = 0.0f;
	distance = distance.Normalize() * -1.0f;	// playerからBoss方向へのベクトルを反対方向にする

	// 最終的な目標地点の座標
	Vector3 targetPos = playerPosition + (distance * param_.getDistance);
	targetPos.y = bossPosition.y;

	// 旋回に必要な情報を計算する
	Vector3 centerPos_ = bossPosition - targetPos;
	centerPos_.y = bossPosition.y;

	Vector3 toBoss = bossPosition - playerPosition;

	int rand = RandomInt(0, 1);
	Vector3 tangent = CVector3::ZERO;
	if (rand == 0) {
		tangent = Vector3::Cross(CVector3::UP, toBoss);
	} else {
		tangent = Vector3::Cross(toBoss, CVector3::UP);
	}

	velocity_ = tangent.Normalize() * param_.moveSpeed;

	pTarget_->SetIsMove(true);
	pTarget_->SetIsAttack(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionStrafe::Update() {
	taskTimer_ += GameTimer::DeltaTime();

	if (!stopping_) {
		Spin();
	} else {
		Stop();
	}

	if (taskTimer_ > param_.moveTime) {
		stopping_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionStrafe::End() {
	pTarget_->SetIsMove(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 旋回処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionStrafe::Spin() {
	Vector3 playerPos = pTarget_->GetPlayerPosition();
	Vector3 toCenter = playerPos - pTarget_->GetPosition();
	float radius = toCenter.Length();
	Vector3 centerDire = toCenter.Normalize();

	// 敵の速度が常に円の接線方向になるように加速度を加える
	float speed = velocity_.Length();
	accel_ = centerDire * (speed * speed / radius);
	accel_.y = 0.0f;

	// 速度と位置を更新
	velocity_ += accel_ * GameTimer::DeltaTime();
	pTarget_->GetTransform()->MoveVelocity(velocity_ * GameTimer::DeltaTime(), 0.1f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 停止処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionStrafe::Stop() {
	velocity_ *= std::exp(-param_.decayRate * GameTimer::DeltaTime());
	pTarget_->GetTransform()->srt_.translate += velocity_ * GameTimer::DeltaTime();

	Quaternion playerToRotate_ = Quaternion::LookAt(pTarget_->GetPosition(), pTarget_->GetPlayerPosition());
	pTarget_->GetTransform()->srt_.rotate = Quaternion::Slerp(pTarget_->GetTransform()->srt_.rotate, playerToRotate_, 0.05f);
}