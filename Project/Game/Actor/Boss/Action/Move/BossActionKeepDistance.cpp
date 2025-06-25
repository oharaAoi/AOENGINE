#include "BossActionKeepDistance.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/Action/BossActionIdle.h"
#include "Engine/Lib/Math/MyRandom.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionKeepDistance::Build() {
	SetName("actionKeepDistance");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionKeepDistance::OnStart() {
	actionTimer_ = 0;

	velocity_ = CVector3::ZERO;

	// 距離を計算する
	Vector3 distance = pOwner_->GetPosition() - pOwner_->GetPlayerPosition();
	distance.y = 0.0f;
	distance = distance.Normalize() * -1.0f;	// playerからBoss方向へのベクトルを反対方向にする

	// 最終的な目標地点の座標
	Vector3 targetPos = pOwner_->GetPlayerPosition() + (distance * param_.getDistance);

	moveType_ = 1;
	if (moveType_ == 0) {	// 直進移動
		accel_ = (targetPos - pOwner_->GetPosition()).Normalize();
	} else if (moveType_ == 1) {	// 旋回移動
		centerPos_ = pOwner_->GetPosition() - targetPos;
		centerPos_.y = pOwner_->GetPosition().y;
		
		Vector3 toBoss = pOwner_->GetPosition() - pOwner_->GetPlayerPosition();
		Vector3 tangent = Vector3::Cross(CVector3::UP, toBoss);

		velocity_ = tangent.Normalize() * param_.moveSpeed;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionKeepDistance::OnUpdate() {
	actionTimer_ += GameTimer::DeltaTime();

	if (moveType_ == 0) {
		Direct();
	} else if (moveType_ == 1) {
		Spin();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionKeepDistance::OnEnd() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionKeepDistance::CheckNextAction() {
	if (actionTimer_ > param_.moveTime) {
		NextAction<BossActionIdle>();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionKeepDistance::IsInput() {
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionKeepDistance::Debug_Gui() {
	ImGui::DragFloat("moveSpeed", &param_.moveSpeed, 0.1f);
	ImGui::DragFloat("moveTime", &param_.moveTime, 0.1f);
	ImGui::DragFloat("getDistance", &param_.getDistance, 0.1f);

	if (ImGui::Button("Save")) {
		JsonItems::Save(pManager_->GetName(), param_.ToJson(param_.GetName()));
	}
}

void BossActionKeepDistance::Direct() {
	velocity_ += accel_ * GameTimer::DeltaTime();
	pOwner_->GetTransform()->MoveVelocity(velocity_ * param_.moveSpeed * GameTimer::DeltaTime(), 0.1f);
}

void BossActionKeepDistance::Spin() {
	Vector3 playerPos = pOwner_->GetPlayerPosition();
	Vector3 toCenter = playerPos - pOwner_->GetPosition();
	float radius = toCenter.Length();
	Vector3 centerDire = toCenter.Normalize();

	// 敵の速度が常に円の接線方向になるように加速度を加える
	float speed = velocity_.Length();
	accel_ = centerDire * (speed * speed / radius);

	// 速度と位置を更新
	velocity_ += accel_ * GameTimer::DeltaTime();
	pOwner_->GetTransform()->MoveVelocity(velocity_ * GameTimer::DeltaTime(), 0.1f);
}
