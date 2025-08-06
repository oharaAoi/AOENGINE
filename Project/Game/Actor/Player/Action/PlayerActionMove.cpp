#include "PlayerActionMove.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
#include "Game/Actor/Player/Action/PlayerActionJump.h"
#include "Game/Actor/Player/Action/PlayerActionQuickBoost.h"
#include "Game/Actor/Player/Action/PlayerActionBoost.h"
#include "Game/Actor/Player/Action/PlayerActionShotRight.h"
#include "Game/Actor/Player/Action/PlayerActionShotLeft.h"
#include "Game/Actor/Player/Action/PlayerActionRightShoulder.h"
#include "Game/Actor/Player/Action/PlayerActionTurnAround.h"
// Engine
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Json/JsonItems.h"

void PlayerActionMove::Debug_Gui() {
	ImGui::DragFloat("speed", &param_.speed, 0.1f);
	ImGui::DragFloat("boostSpeed", &param_.boostSpeed, 0.1f);
	ImGui::DragFloat("moveT", &param_.moveT, 0.01f);
	ImGui::DragFloat("rotateT", &param_.rotateT, 0.01f);
	ImGui::DragFloat("decayRate", &param_.decayRate, 0.1f);
	param_.moveT = std::clamp(param_.moveT, 0.0f, 1.0f);
	param_.rotateT = std::clamp(param_.rotateT, 0.0f, 1.0f);

	if (ImGui::Button("Save")) {
		JsonItems::Save(pManager_->GetName(), param_.ToJson(param_.GetName()));
	}
	if (ImGui::Button("Apply")) {
		param_.FromJson(JsonItems::GetData(pManager_->GetName(), param_.GetName()));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::Build() {
	SetName("actionMove");
	param_.FromJson(JsonItems::GetData(pManager_->GetName(), param_.GetName()));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::OnStart() {
	actionTimer_ = 0;

	if (pOwner_->GetIsBoostMode() || !pOwner_->GetIsLanding()) {
		pOwner_->GetJetEngine()->JetIsStart();
	} else {
		pOwner_->GetJetEngine()->JetIsStop();
	}

	pOwner_->SetIsMoving(true);

	isTurnAround_ = false;

	inputStick_ = Input::GetInstance()->GetLeftJoyStick(kDeadZone_).Normalize();
	Vector3 dire = pOwner_->GetFollowCamera()->GetAngleX().Rotate(Vector3{ inputStick_.x, 0.0f, inputStick_.y });
	if (pOwner_->GetIsBoostMode()) {
		accel_ = dire * param_.boostSpeed;
	} else {
		accel_ = dire * param_.speed;
	}
	velocity_ += accel_;

	if (pOwner_->GetIsBoostMode()) {
		AnimationClip* clip = pOwner_->GetGameObject()->GetAnimetor()->GetAnimationClip();
		clip->PoseToAnimation("move", 0.1f);
		clip->SetIsLoop(false);
	} else {
		AnimationClip* clip = pOwner_->GetGameObject()->GetAnimetor()->GetAnimationClip();
		clip->PoseToAnimation("walk", 0.1f);
		clip->SetIsLoop(true);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::OnUpdate() {
	actionTimer_ += GameTimer::DeltaTime();

	if (preBoost_ != pOwner_->GetIsBoostMode()) {
		if (preBoost_) {
			AnimationClip* clip = pOwner_->GetGameObject()->GetAnimetor()->GetAnimationClip();
			clip->PoseToAnimation("walk", 0.1f);
			clip->SetIsLoop(true);
		} else {
			AnimationClip* clip = pOwner_->GetGameObject()->GetAnimetor()->GetAnimationClip();
			clip->PoseToAnimation("move", 0.1f);
			clip->SetIsLoop(false);
		}
	}

	Move();

	// 移動の入力がなかったら回復処理を行っていく
	if (pOwner_->GetIsLanding()) {
		pOwner_->RecoveryEN(actionTimer_);
	}

	preBoost_ = pOwner_->GetIsBoostMode();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::OnEnd() {
	pOwner_->SetIsMoving(false);
	if (pOwner_->GetIsBoostMode()) {
		AnimationClip* clip = pOwner_->GetGameObject()->GetAnimetor()->GetAnimationClip();
		clip->PoseToAnimation("move_cancel", 0.1f);
		clip->SetIsLoop(false);
	} else {
		AnimationClip* clip = pOwner_->GetGameObject()->GetAnimetor()->GetAnimationClip();
		clip->PoseToAnimation("idle", 0.1f);
		clip->SetIsLoop(false);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次に行うアクションの判定
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::CheckNextAction() {
	if (velocity_.Length() <= 0.1f) {
		if (pOwner_->GetIsLanding()) {
			NextAction<PlayerActionIdle>();
		}
	}

	//if (isTurnAround_) {
	//	// 反対を向く処理をす
	//	if (pOwner_->GetIsBoostMode()) {
	//		context_->Set<float>("speed", param_.boostSpeed);
	//	} else {
	//		context_->Set<float>("speed", param_.speed);
	//	}
	//	NextAction<PlayerActionTurnAround>();
	//}

	if (CheckInput<PlayerActionJump>()) {
		AddAction<PlayerActionJump>();
	}

	if (CheckInput<PlayerActionShotRight>()) {
		AddAction<PlayerActionShotRight>();
	}

	if (CheckInput<PlayerActionShotLeft>()) {
		AddAction<PlayerActionShotLeft>();
	}

	if (CheckInput<PlayerActionRightShoulder>()) {
		AddAction<PlayerActionRightShoulder>();
	}


	if (CheckInput<PlayerActionQuickBoost>()) {
		NextAction<PlayerActionQuickBoost>();
	}

	if (CheckInput<PlayerActionBoost>()) {
		NextAction<PlayerActionBoost>();
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerActionMove::IsInput() {
	Vector2 current = Input::GetInstance()->GetLeftJoyStick(kDeadZone_);
	if (current.x != 0.0f || current.y != 0.0f) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ main action
///////////////////////////////////////////////////////////////////////////////////////////////


void PlayerActionMove::Move() {
	WorldTransform* transform = pOwner_->GetTransform();
	inputStick_ = Input::GetInstance()->GetLeftJoyStick(kDeadZone_).Normalize();

	// speedを設定
	float speed = 0.0f;
	if (pOwner_->GetIsBoostMode()) {
		speed = param_.boostSpeed;
	} else {
		speed = param_.speed;
	}

	// 加速方向を計算する
	Vector3 dire = pOwner_->GetFollowCamera()->GetAngleX().Rotate(Vector3{ inputStick_.x, 0.0f, inputStick_.y });
	accel_ = dire * speed;
	
	// velocityを加速方向へなじませる
	velocity_ = Vector3::Lerp(velocity_, accel_, param_.moveT);
	velocity_ += accel_ * GameTimer::DeltaTime();
	// 最大速度クランプ
	if (velocity_.Length() > param_.boostSpeed) {
		velocity_ = velocity_.Normalize() * param_.boostSpeed;
	}

	// 減速処理
	if (inputStick_.Length() <= 0.01f) {
		velocity_ *= std::exp(-param_.decayRate * GameTimer::DeltaTime());
	}

	transform->translate_ += velocity_ * GameTimer::DeltaTime();
	
	// playerを移動方向に向ける
	if (velocity_.x != 0.0f || velocity_.z != 0.0f) {
		float angle = std::atan2f(velocity_.x, velocity_.z);
		Quaternion lerpQuaternion = Quaternion::Slerp(transform->rotation_, Quaternion::AngleAxis(angle, CVector3::UP), param_.rotateT);
		transform->rotation_ = lerpQuaternion;
	}

	if (pOwner_->GetIsBoostMode() || !pOwner_->GetIsLanding()) {
		pOwner_->GetJetEngine()->JetIsStart();
	} else {
		pOwner_->GetJetEngine()->JetIsStop();
	}

	preVelocity_ = velocity_;

	// 方向転換をしなかったら
	//if (IsDirectionReversed(accel)) {
	//	preVelocity_ = velocity_;
	//	//isTurnAround_ = true;
	//	return;
	//}
}

bool PlayerActionMove::IsDirectionReversed(const Vector3& currentVelocity) {
	if (currentVelocity.Length() > 0.3f || preVelocity_.Length() > 0.3f) {
		float dot = Vector3::Dot(currentVelocity, preVelocity_);
		if (dot < -0.5f) {
			return true;
		}
	}
	return false;
}
