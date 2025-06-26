#include "BossActionStrafe.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Actor/Boss/Action/BossActionIdle.h"
#include "Game/Actor/Boss/Boss.h"
#include "Engine/Lib/Math/MyRandom.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionStrafe::Build() {
	SetName("actionStrafe");
	param_.FromJson(JsonItems::GetData(pManager_->GetName(), param_.GetName()));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionStrafe::OnStart() {
	actionTimer_ = 0;
	stopping_ = false;
	velocity_ = CVector3::ZERO;

	Vector3 bossPosition = pOwner_->GetPosition();
	Vector3 playerPosition = pOwner_->GetPlayerPosition();

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
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionStrafe::OnUpdate() {
	actionTimer_ += GameTimer::DeltaTime();

	if (!stopping_) {
		Spin();
	} else {
		Stop();
	}

	if (actionTimer_ > param_.moveTime) {
		stopping_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionStrafe::OnEnd() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次の行動の確認
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionStrafe::CheckNextAction() {
	if (stopping_) {
		if (velocity_.Length() <= 1.0f) {
			NextAction<BossActionIdle>();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionStrafe::IsInput() {
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionStrafe::Debug_Gui() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 旋回処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionStrafe::Spin() {
	Vector3 playerPos = pOwner_->GetPlayerPosition();
	Vector3 toCenter = playerPos - pOwner_->GetPosition();
	float radius = toCenter.Length();
	Vector3 centerDire = toCenter.Normalize();

	// 敵の速度が常に円の接線方向になるように加速度を加える
	float speed = velocity_.Length();
	accel_ = centerDire * (speed * speed / radius);
	accel_.y = 0.0f;

	// 速度と位置を更新
	velocity_ += accel_ * GameTimer::DeltaTime();
	pOwner_->GetTransform()->MoveVelocity(velocity_ * GameTimer::DeltaTime(), 0.1f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 停止処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionStrafe::Stop() {
	velocity_ *= std::exp(-param_.decayRate * GameTimer::DeltaTime());
	pOwner_->GetTransform()->translate_ += velocity_ * GameTimer::DeltaTime();

	Quaternion playerToRotate_ = Quaternion::LookAt(pOwner_->GetPosition(), pOwner_->GetPlayerPosition());
	pOwner_->GetTransform()->rotation_ = Quaternion::Slerp(pOwner_->GetTransform()->rotation_, playerToRotate_, 0.05f);
}