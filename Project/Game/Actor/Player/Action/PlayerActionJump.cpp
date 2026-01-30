#include "PlayerActionJump.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
#include "Game/Actor/Player/Action/PlayerActionMove.h"
#include "Game/Actor/Player/Action/PlayerActionRightShoulder.h"
// Engine
#include "Engine/Lib/GameTimer.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Module/Components/Animation/AnimationClip.h"
#include "Engine/System/Manager/ParticleManager.h"

void PlayerActionJump::Debug_Gui() {
	Math::Vector3 vel = pRigidbody_->GetVelocity();
	ImGui::Text("vel: (%.2f, %.2f, %.2f)", vel.x, vel.y, vel.z);
	param_.Debug_Gui();
}

void PlayerActionJump::Parameter::Debug_Gui() {
	ImGui::DragFloat("jumpForce", &jumpForce, 0.1f);
	ImGui::DragFloat("finishChargeTime", &finishChargeTime, 0.1f);
	ImGui::DragFloat("risingForce", &risingForce, 0.1f);
	ImGui::DragFloat("maxAcceleration", &maxAcceleration, 0.1f);
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

	AOENGINE::ParticleManager* manager = AOENGINE::ParticleManager::GetInstance();
	jetBurnLeft_ = manager->CreateParticle("Jet");
	jetBurnRight_ = manager->CreateParticle("Jet");

	AOENGINE::Skeleton* skeleton = pOwner_->GetGameObject()->GetAnimator()->GetSkeleton();
	feetMatrixLeft_ = skeleton->GetSkeletonSpaceMat("left_feetFront") * pOwnerTransform_->GetWorldMatrix();
	feetMatrixRight_ = skeleton->GetSkeletonSpaceMat("right_feetFront") * pOwnerTransform_->GetWorldMatrix();

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

	pRigidbody_->SetVelocityY(param_.jumpForce);
	mainAction_ = std::bind(&PlayerActionJump::Jump, this);
	
	pOwner_->SetIsLanding(false);
	
	pOwner_->GetFollowCamera()->SetShake(param_.cameraShakeTime, param_.cameraShakeStrength);
	pOwner_->GetJetEngine()->JetIsStart();
	pOwner_->ConsumeEN(param_.jumpEnergy);

	AOENGINE::AnimationClip* clip = pOwner_->GetGameObject()->GetAnimator()->GetAnimationClip();
	clip->PoseToAnimation("jump", param_.animationBlendTime);
	clip->SetIsLoop(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::OnUpdate() {
	actionTimer_ += AOENGINE::GameTimer::DeltaTime();

	mainAction_();

	AOENGINE::Skeleton* skeleton = pOwner_->GetGameObject()->GetAnimator()->GetSkeleton();
	feetMatrixLeft_ = skeleton->GetSkeletonSpaceMat("left_feetFront") * pOwnerTransform_->GetWorldMatrix();
	feetMatrixRight_ = skeleton->GetSkeletonSpaceMat("right_feetFront") * pOwnerTransform_->GetWorldMatrix();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::OnEnd() {
	AOENGINE::AnimationClip* clip = pOwner_->GetGameObject()->GetAnimator()->GetAnimationClip();
	clip->PoseToAnimation("landing", param_.animationBlendTime);
	clip->SetIsLoop(false);

	jetBurnLeft_->SetIsStop(true);
	jetBurnRight_->SetIsStop(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次に行うアクションの判定
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::CheckNextAction() {
	if (pOwner_->GetParam().energy <= 0.0f) {
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
	if (pOwner_->GetParam().energy < param_.jumpEnergy) {
		return false;
	}

	if (AOENGINE::Input::GetInstance()->IsTriggerButton(XInputButtons::ButtonA)) {
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ main action
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::Jump() {
	jetBurnLeft_->SetIsStop(false);
	jetBurnRight_->SetIsStop(false);

	if (pRigidbody_->GetVelocity().y <= 0.1f) {
		mainAction_ = std::bind(&PlayerActionJump::Rising, this);
		pOwner_->SetIsLanding(false);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 上昇を行う
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::Rising() {
	if (pOwner_->GetParam().energy < param_.jumpEnergy * AOENGINE::GameTimer::DeltaTime()) {
		jetBurnLeft_->SetIsStop(true);
		jetBurnRight_->SetIsStop(true);
		pOwner_->GetGameObject()->GetRigidbody()->SetGravity(true);
		return;
	}

	// ボタンを押していたら上昇する
	if (AOENGINE::Input::GetInstance()->IsPressButton(XInputButtons::ButtonA)) {
		jetBurnLeft_->SetIsStop(false);
		jetBurnRight_->SetIsStop(false);

		pOwner_->GetJetEngine()->JetIsStart();
		pOwner_->GetGameObject()->GetRigidbody()->SetGravity(false);

		pOwner_->ConsumeEN(param_.jumpEnergy * AOENGINE::GameTimer::DeltaTime());
		pRigidbody_->SetVelocityY(param_.risingForce);

	} else {
		jetBurnLeft_->SetIsStop(true);
		jetBurnRight_->SetIsStop(true);

		pOwner_->GetJetEngine()->JetIsStop();
		pOwner_->GetGameObject()->GetRigidbody()->SetGravity(true);
	}
}