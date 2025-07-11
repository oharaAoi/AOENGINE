#include "PlayerActionJump.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
#include "Game/Actor/Player/Action/PlayerActionMove.h"
// Engine
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Manager/ParticleManager.h"

void PlayerActionJump::Debug_Gui() {
	ImGui::DragFloat("smallJumpTime", &smallJumpTime_, 0.1f);

	ImGui::DragFloat("jumpForce", &param_.jumpForce, 0.1f);
	ImGui::DragFloat("risingForce", &param_.risingForce, 0.1f);
	ImGui::DragFloat("maxAcceleration", &param_.maxAcceleration, 0.1f);
	ImGui::DragFloat("accelDecayRate", &param_.accelDecayRate, 0.1f);
	ImGui::DragFloat("velocityDecayRate", &param_.velocityDecayRate, 0.1f);
	ImGui::DragFloat("jumpEnergy", &param_.jumpEnergy, 0.1f);
	ImGui::DragFloat("cameraShakeTime", &param_.cameraShakeTime, 0.1f);
	ImGui::DragFloat("cameraShakeStrength", &param_.cameraShakeStrength, 0.1f);
	
	if (ImGui::Button("Save")) {
		JsonItems::Save(pManager_->GetName(), param_.ToJson(param_.GetName()));
	}
	if (ImGui::Button("Apply")) {
		param_.FromJson(JsonItems::GetData(pManager_->GetName(), param_.GetName()));
	}
	ImGui::Text("acceleration: (%.2f, %.2f, %.2f)", acceleration_.x, acceleration_.y, acceleration_.z);
	ImGui::Text("velocity: (%.2f, %.2f, %.2f)", velocity_.x, velocity_.y, velocity_.z);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::Build() {
	SetName("ActionJump");
	pOwnerTransform_ = pOwner_->GetTransform();
	param_.FromJson(JsonItems::GetData(pManager_->GetName(), param_.GetName()));

	ParticleManager* manager = ParticleManager::GetInstance();
	jetBurnLeft_ = manager->CrateParticle("legsJet");
	jetBurnRight_ = manager->CrateParticle("legsJet");

	Skeleton* skeleton = pOwner_->GetGameObject()->GetAnimetor()->GetSkeleton();
	feetMatrixLeft_ = skeleton->GetSkeltonSpaceMat("left_feetFront") * pOwnerTransform_->GetWorldMatrix();
	feetMatrixRight_ = skeleton->GetSkeltonSpaceMat("right_feetFront") * pOwnerTransform_->GetWorldMatrix();

	jetBurnLeft_->SetParent(feetMatrixLeft_);
	jetBurnRight_->SetParent(feetMatrixRight_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::OnStart() {
	actionTimer_ = 0.0f;
	
	// ジャンプした分のエネルギーを消費しておく
	pOwner_->ConsumeEN(param_.jumpEnergy);

	acceleration_.y = param_.jumpForce;
	velocity_ = acceleration_;
	mainAction_ = std::bind(&PlayerActionJump::Jump, this);
	
	pOwner_->SetIsLanding(false);
	pOwner_->GetGameObject()->GetRigidbody()->SetGravity(false);

	pOwner_->GetFollowCamera()->SetShake(param_.cameraShakeTime, param_.cameraShakeStrength);
	pOwner_->GetJetEngine()->JetIsStart();
	pOwner_->ConsumeEN(param_.jumpEnergy);

	
	//pOwner_->GetGameObject()->GetAnimetor()->TransitionAnimation("jump", 0.5f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::OnUpdate() {
	actionTimer_ += GameTimer::DeltaTime();

	mainAction_();

	pOwnerTransform_->translate_ += velocity_ * GameTimer::DeltaTime();

	Skeleton* skeleton = pOwner_->GetGameObject()->GetAnimetor()->GetSkeleton();
	feetMatrixLeft_ = skeleton->GetSkeltonSpaceMat("left_feetFront") * pOwnerTransform_->GetWorldMatrix();
	feetMatrixRight_ = skeleton->GetSkeltonSpaceMat("right_feetFront") * pOwnerTransform_->GetWorldMatrix();

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::OnEnd() {
	
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次に行うアクションの判定
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::CheckNextAction() {
	if (pOwner_->GetIsLanding()) {
		NextAction<PlayerActionIdle>();
	}

	if (CheckInput<PlayerActionMove>()) {
		AddAction<PlayerActionMove>();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerActionJump::IsInput() {
	if (Input::GetInstance()->GetIsPadTrigger(XInputButtons::BUTTON_A)) {
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

	velocity_ += acceleration_ * GameTimer::DeltaTime();
	acceleration_ *= std::exp(-param_.accelDecayRate * GameTimer::DeltaTime());

	if (acceleration_.Length() <= 0.1f) {
		mainAction_ = std::bind(&PlayerActionJump::Rising, this);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 上昇を行う
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::Rising() {
	// ボタンを押していたら上昇する
	if (Input::GetInstance()->GetPressPadTrigger(XInputButtons::BUTTON_A)) {
		jetBurnLeft_->SetIsStop(false);
		jetBurnRight_->SetIsStop(false);

		acceleration_.y = param_.risingForce;
		pOwner_->GetJetEngine()->JetIsStart();
		pOwner_->GetGameObject()->GetRigidbody()->SetGravity(false);

		pOwner_->ConsumeEN(param_.jumpEnergy * GameTimer::DeltaTime());
		velocity_ = acceleration_;

	} else {
		jetBurnLeft_->SetIsStop(true);
		jetBurnRight_->SetIsStop(true);

		acceleration_.y = 0.0f;
		pOwner_->GetJetEngine()->JetIsStop();
		pOwner_->GetGameObject()->GetRigidbody()->SetGravity(true);

		velocity_ *= std::exp(-param_.velocityDecayRate * GameTimer::DeltaTime());
	}

}
