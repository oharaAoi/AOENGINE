#include "PlayerActionJump.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
#include "Game/Actor/Player/Action/PlayerActionMove.h"
#include "Game/Actor/Player/Action/PlayerActionRightShoulder.h"
// Engine
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Manager/ParticleManager.h"

void PlayerActionJump::Debug_Gui() {
	ImGui::Text("acceleration: (%.2f, %.2f, %.2f)", acceleration_.x, acceleration_.y, acceleration_.z);
	ImGui::DragFloat("smallJumpTime", &smallJumpTime_, 0.1f);

	param_.Debug_Gui();
}

void PlayerActionJump::Parameter::Debug_Gui() {
	ImGui::DragFloat("jumpForce", &jumpForce, 0.1f);
	ImGui::DragFloat("chargeTime", &chargeTime, 0.1f);
	ImGui::DragFloat("finishChargeTime", &finishChargeTime, 0.1f);
	ImGui::DragFloat("risingForce", &risingForce, 0.1f);
	ImGui::DragFloat("maxAcceleration", &maxAcceleration, 0.1f);
	ImGui::DragFloat("accelDecayRate", &accelDecayRate, 0.1f);
	ImGui::DragFloat("velocityDecayRate", &velocityDecayRate, 0.1f);
	ImGui::DragFloat("jumpEnergy", &jumpEnergy, 0.1f);
	ImGui::DragFloat("cameraShakeTime", &cameraShakeTime, 0.1f);
	ImGui::DragFloat("cameraShakeStrength", &cameraShakeStrength, 0.1f);
	ImGui::DragFloat("animationBlendTime", &animationBlendTime, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::Build() {
	SetName("ActionJump");
	pOwnerTransform_ = pOwner_->GetTransform();

	param_.SetGroupName(pManager_->GetName());
	param_.Load();

	ParticleManager* manager = ParticleManager::GetInstance();
	jetBurnLeft_ = manager->CrateParticle("Jet");
	jetBurnRight_ = manager->CrateParticle("Jet");

	Skeleton* skeleton = pOwner_->GetGameObject()->GetAnimetor()->GetSkeleton();
	feetMatrixLeft_ = skeleton->GetSkeltonSpaceMat("left_feetFront") * pOwnerTransform_->GetWorldMatrix();
	feetMatrixRight_ = skeleton->GetSkeltonSpaceMat("right_feetFront") * pOwnerTransform_->GetWorldMatrix();

	jetBurnLeft_->SetParent(feetMatrixLeft_);
	jetBurnRight_->SetParent(feetMatrixRight_);

	pRigidbody_ = pOwner_->GetGameObject()->GetRigidbody();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::OnStart() {
	actionTimer_ = 0.0f;
	
	// ジャンプした分のエネルギーを消費しておく
	pOwner_->ConsumeEN(param_.jumpEnergy);

	acceleration_.y = param_.jumpForce;
	mainAction_ = std::bind(&PlayerActionJump::Charge, this);
	
	pOwner_->SetIsLanding(false);
	pOwner_->GetGameObject()->GetRigidbody()->SetGravity(false);

	pOwner_->GetFollowCamera()->SetShake(param_.cameraShakeTime, param_.cameraShakeStrength);
	pOwner_->GetJetEngine()->JetIsStart();
	pOwner_->ConsumeEN(param_.jumpEnergy);

	AnimationClip* clip = pOwner_->GetGameObject()->GetAnimetor()->GetAnimationClip();
	clip->PoseToAnimation("jump", param_.animationBlendTime);
	clip->SetIsLoop(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::OnUpdate() {
	actionTimer_ += AOENGINE::GameTimer::DeltaTime();

	mainAction_();

	Skeleton* skeleton = pOwner_->GetGameObject()->GetAnimetor()->GetSkeleton();
	feetMatrixLeft_ = skeleton->GetSkeltonSpaceMat("left_feetFront") * pOwnerTransform_->GetWorldMatrix();
	feetMatrixRight_ = skeleton->GetSkeltonSpaceMat("right_feetFront") * pOwnerTransform_->GetWorldMatrix();

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::OnEnd() {
	AnimationClip* clip = pOwner_->GetGameObject()->GetAnimetor()->GetAnimationClip();
	clip->PoseToAnimation("landing", param_.animationBlendTime);
	clip->SetIsLoop(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次に行うアクションの判定
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::CheckNextAction() {
	if (pOwner_->GetParam().energy <= 0.0f) {
		pOwner_->GetGameObject()->GetRigidbody()->SetGravity(true);
		NextAction<PlayerActionIdle>();
	}

	if (actionTimer_ > param_.finishChargeTime) {
		if (pOwner_->GetIsLanding()) {
			NextAction<PlayerActionIdle>();
		}

		if (CheckInput<PlayerActionMove>()) {
			AddAction<PlayerActionMove>();
		}
	}

	if (CheckInput<PlayerActionRightShoulder>()) {
		AddAction<PlayerActionRightShoulder>();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerActionJump::IsInput() {
	if (Input::GetInstance()->IsTriggerButton(XInputButtons::ButtonA)) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ main action
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::SmallJump() {
	if (actionTimer_ > smallJumpTime_) {
		mainAction_ = std::bind(&PlayerActionJump::Jump, this);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ main action
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::Jump() {
	jetBurnLeft_->SetIsStop(false);
	jetBurnRight_->SetIsStop(false);

	pRigidbody_->SetVelocityY(acceleration_.y);
	acceleration_ *= std::exp(-param_.accelDecayRate);

	if (acceleration_.Length() <= 0.1f) {
		mainAction_ = std::bind(&PlayerActionJump::Rising, this);
		pOwner_->SetIsLanding(false);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 上昇を行う
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::Rising() {
	// ボタンを押していたら上昇する
	if (Input::GetInstance()->IsPressButton(XInputButtons::ButtonA)) {
		jetBurnLeft_->SetIsStop(false);
		jetBurnRight_->SetIsStop(false);

		acceleration_.y = param_.risingForce;
		pOwner_->GetJetEngine()->JetIsStart();
		pOwner_->GetGameObject()->GetRigidbody()->SetGravity(false);

		pOwner_->ConsumeEN(param_.jumpEnergy * AOENGINE::GameTimer::DeltaTime());
		pRigidbody_->SetVelocityY(acceleration_.y);

	} else {
		jetBurnLeft_->SetIsStop(true);
		jetBurnRight_->SetIsStop(true);

		pOwner_->GetJetEngine()->JetIsStop();
		pOwner_->GetGameObject()->GetRigidbody()->SetGravity(true);
	}
}

void PlayerActionJump::Charge() {
	if (actionTimer_ > param_.chargeTime) {
		mainAction_ = std::bind(&PlayerActionJump::Jump, this);
		pOwner_->GetGameObject()->GetRigidbody()->SetGravity(false);
		pOwner_->SetIsLanding(false);
	}
}
