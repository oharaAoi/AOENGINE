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
	param_.FromJson(JsonItems::GetData(pManager_->GetName(), param_.GetName()));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionKeepDistance::OnStart() {
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
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionKeepDistance::OnUpdate() {
	actionTimer_ += GameTimer::DeltaTime();

	if (!stopping_) {
		if (moveType_ == 0) {
			Direct();
		} else if (moveType_ == 1) {
			Spin();
		}
	} else {
		Stop();
	}

	if (actionTimer_ > param_.moveTime) {
		stopping_ = true;
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
	if (stopping_) {
		if (velocity_.Length() <= 1.0f) {
			size_t hash = pOwner_->GetAI()->AttackActionAI();
			NextAction(hash);
		}
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
	ImGui::DragFloat("decayRate", &param_.decayRate, 0.1f);

	if (ImGui::Button("Save")) {
		JsonItems::Save(pManager_->GetName(), param_.ToJson(param_.GetName()));
	}
}

void BossActionKeepDistance::Direct() {
	velocity_ += accel_ * GameTimer::DeltaTime();
	pOwner_->GetTransform()->MoveVelocity(velocity_ * GameTimer::DeltaTime(), 0.1f);
}

void BossActionKeepDistance::Spin() {
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

void BossActionKeepDistance::Stop() {
	velocity_ *= std::exp(-param_.decayRate * GameTimer::DeltaTime());
	pOwner_->GetTransform()->translate_ += velocity_ * GameTimer::DeltaTime();

	Quaternion playerToRotate_ = Quaternion::LookAt(pOwner_->GetPosition(), pOwner_->GetPlayerPosition());
	pOwner_->GetTransform()->rotation_ = Quaternion::Slerp(pOwner_->GetTransform()->rotation_, playerToRotate_, 0.05f);

}
