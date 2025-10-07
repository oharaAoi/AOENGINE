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
	param_.Debug_Gui();
	param_.moveT = std::clamp(param_.moveT, 0.0f, 1.0f);
	param_.rotateT = std::clamp(param_.rotateT, 0.0f, 1.0f);
}

void PlayerActionMove::Parameter::Debug_Gui() {
	ImGui::DragFloat("speed", &speed, 0.1f);
	ImGui::DragFloat("boostSpeed", &boostSpeed, 0.1f);
	ImGui::DragFloat("moveT", &moveT, 0.01f);
	ImGui::DragFloat("rotateT", &rotateT, 0.01f);
	ImGui::DragFloat("decayRate", &decayRate, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::Build() {
	SetName("actionMove");

	param_.SetGroupName(pManager_->GetName());
	param_.Load();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::OnStart() {
	actionTimer_ = 0;
	pRigidbody_ = pOwner_->GetGameObject()->GetRigidbody();

	pOwner_->SetIsMoving(true);
	isTurnAround_ = false;

	inputStick_ = Input::GetInstance()->GetLeftJoyStick(kDeadZone_).Normalize();
	Vector3 dire = pOwner_->GetFollowCamera()->GetAngleX().Rotate(Vector3{ inputStick_.x, 0.0f, inputStick_.y });
	if (pOwner_->GetIsBoostMode()) {
		accel_ = dire * param_.boostSpeed;
	} else {
		accel_ = dire * param_.speed;
	}
	velocity_ = accel_ + pRigidbody_->GetVelocity();

	// ----------------------
	// ↓ Animationの設定
	// ----------------------
	if (pOwner_->GetIsBoostMode()) {
		AnimationClip* clip = pOwner_->GetGameObject()->GetAnimetor()->GetAnimationClip();
		clip->PoseToAnimation("move", 0.1f);
		clip->SetIsLoop(false);
	} else {
		AnimationClip* clip = pOwner_->GetGameObject()->GetAnimetor()->GetAnimationClip();
		clip->PoseToAnimation("walk", 0.1f);
		clip->SetIsLoop(true);
	}

	// ----------------------
	// ↓ boostの設定
	// ----------------------
	if (pOwner_->GetIsBoostMode() || !pOwner_->GetIsLanding()) {
		pOwner_->GetJetEngine()->JetIsStart();
	} else {
		pOwner_->GetJetEngine()->JetIsStop();
	}

	pOwner_->UpdateJoint();
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
	if (pRigidbody_->GetVelocity().Length() <= 0.01f) {
		if (pOwner_->GetIsLanding()) {
			NextAction<PlayerActionIdle>();
		}
	}

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

	/*if (CheckInput<PlayerActionBoost>()) {
		NextAction<PlayerActionBoost>();
	}*/

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerActionMove::IsInput() {
	if (pManager_->ExistAction(typeid(PlayerActionQuickBoost).hash_code())) {
		return false;
	}

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


	// ----------------------
	// ↓ speedを設定
	// ----------------------
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
	if (inputStick_.Length() <= 0.01f) {
		velocity_ *= std::exp(-param_.decayRate * GameTimer::DeltaTime());
	}

	pRigidbody_->AddVelocityX(velocity_.x * GameTimer::DeltaTime());
	pRigidbody_->AddVelocityZ(velocity_.z * GameTimer::DeltaTime());

	float length = pRigidbody_->GetVelocity().Length();
	if (length > speed) {
		pRigidbody_->SetVelocity(velocity_.Normalize() * speed);
	}

	// playerを移動方向に向ける
	if (velocity_.x != 0.0f || velocity_.z != 0.0f) {
		float angle = std::atan2f(velocity_.x, velocity_.z);
		Quaternion lerpQuaternion = Quaternion::Slerp(transform->srt_.rotate, Quaternion::AngleAxis(angle, CVector3::UP), param_.rotateT);
		transform->srt_.rotate = lerpQuaternion;
	}

	if (pOwner_->GetIsBoostMode() || !pOwner_->GetIsLanding()) {
		pOwner_->GetJetEngine()->JetIsStart();
	} else {
		pOwner_->GetJetEngine()->JetIsStop();
	}

	preVelocity_ = velocity_;
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
