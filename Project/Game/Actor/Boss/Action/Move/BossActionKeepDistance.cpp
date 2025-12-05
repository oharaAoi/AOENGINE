#include "BossActionKeepDistance.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Actor/Boss/Boss.h"
#include "Engine/Lib/Math/MyRandom.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行処理
///////////////////////////////////////////////////////////////////////////////////////////////

BehaviorStatus BossActionKeepDistance::Execute() {
	return Action();
}

float BossActionKeepDistance::EvaluateWeight() {
	return 0.5f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionKeepDistance::Debug_Gui() {
	BaseTaskNode::Debug_Gui();
	param_.Debug_Gui();
}

void BossActionKeepDistance::Parameter::Debug_Gui() {
	ImGui::DragFloat("moveSpeed", &moveSpeed, 0.1f);
	ImGui::DragFloat("moveTime", &moveTime, 0.1f);
	ImGui::DragFloat("getDistance", &getDistance, 0.1f);
	ImGui::DragFloat("decayRate", &decayRate, 0.1f);
	ImGui::DragFloat("rotateT", &rotateT, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionKeepDistance::IsFinish() {
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

bool BossActionKeepDistance::CanExecute() {
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionKeepDistance::Init() {
	param_.SetGroupName("BossAction");
	param_.Load();

	taskTimer_ = 0;
	stopping_ = false;

	velocity_ = CVector3::ZERO;

	Vector3 bossPosition = pTarget_->GetPosition();
	Vector3 playerPosition = pTarget_->GetTargetPos();

	// 距離を計算する
	Vector3 distance = bossPosition - playerPosition;
	distance.y = 0.0f;
	distance = distance.Normalize() * -1.0f;	// playerからBoss方向へのベクトルを反対方向にする

	// 最終的な目標地点の座標
	Vector3 targetPos = playerPosition + (distance * param_.getDistance);
	targetPos.y = bossPosition.y;

	moveType_ = RandomInt(0, 1);
	if (moveType_ == 0) {	// 直進移動
		accel_ = (targetPos - bossPosition).Normalize() * param_.moveSpeed;
	} else if (moveType_ == 1) {	// 旋回移動
		centerPos_ = bossPosition - targetPos;
		centerPos_.y = bossPosition.y;

		Vector3 toBoss = bossPosition - playerPosition;
		Vector3 tangent = Vector3::Cross(CVector3::UP, toBoss);

		velocity_ = tangent.Normalize() * param_.moveSpeed;
	}

	pTarget_->SetIsMove(true);
	pTarget_->SetIsAttack(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionKeepDistance::Update() {
	taskTimer_ += GameTimer::DeltaTime();

	if (!stopping_) {
		if (moveType_ == 0) {
			Direct();
		} else if (moveType_ == 1) {
			Spin();
		}
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

void BossActionKeepDistance::End() {
	pTarget_->SetIsMove(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ user Function
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionKeepDistance::Direct() {
	velocity_ += accel_ * GameTimer::DeltaTime();
	pTarget_->GetTransform()->MoveVelocity(velocity_ * GameTimer::DeltaTime(), param_.rotateT);
}

void BossActionKeepDistance::Spin() {
	Vector3 playerPos = pTarget_->GetTargetPos();
	Vector3 toCenter = playerPos - pTarget_->GetPosition();
	float radius = toCenter.Length();
	Vector3 centerDire = toCenter.Normalize();

	// 敵の速度が常に円の接線方向になるように加速度を加える
	float speed = velocity_.Length();
	accel_ = centerDire * (speed * speed / radius);
	accel_.y = 0.0f;

	// 速度と位置を更新
	velocity_ += accel_ * GameTimer::DeltaTime();
	pTarget_->GetTransform()->MoveVelocity(velocity_ * GameTimer::DeltaTime(), param_.rotateT);
}

void BossActionKeepDistance::Stop() {
	velocity_ *= std::exp(-param_.decayRate * GameTimer::DeltaTime());
	pTarget_->GetTransform()->srt_.translate += velocity_ * GameTimer::DeltaTime();

	Quaternion playerToRotate_ = Quaternion::LookAt(pTarget_->GetPosition(), pTarget_->GetTargetPos());
	pTarget_->GetTransform()->srt_.rotate = Quaternion::Slerp(pTarget_->GetTransform()->GetRotate(), playerToRotate_, param_.rotateT);

}
