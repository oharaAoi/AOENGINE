#include "PlayerActionBoost.h"
#include "Engine/Lib/GameTimer.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionMove.h"
#include "Game/Actor/Player/Action/PlayerActionQuickBoost.h"

PlayerActionBoost::~PlayerActionBoost() {
	blur_.reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::Debug_Gui() {
	initialPram_.Debug_Gui();
}

void PlayerActionBoost::Parameter::Debug_Gui() {
	ImGui::DragFloat("chargeTime", &chargeTime, .1f);
	ImGui::DragFloat("boostForce", &boostForce, .1f);
	ImGui::DragFloat("stopForce", &stopForce, .1f);
	ImGui::DragFloat("bluerStrength", &bluerStrength, .1f);
	ImGui::DragFloat("bluerStartTime", &bluerStartTime, .1f);
	ImGui::DragFloat("bluerStopTime", &bluerStopTime, .1f);
	ImGui::DragFloat("consumeEN", &consumeEnergy, .1f);
	ImGui::DragFloat("overTakingThreshold", &overTakingThreshold, .1f);
	ImGui::DragFloat("stopThreshold", &stopThreshold, .1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::Build() {
	SetName("ActionBoost");
	pInput_ = AOENGINE::Input::GetInstance();
	pOwnerTransform_ = pOwner_->GetTransform();
	pRigidbody_ = pOwner_->GetGameObject()->GetRigidbody();

	initialPram_.SetGroupName(pManager_->GetName());
	initialPram_.Load();

	blur_ = Engine::GetPostProcess()->GetRadialBlur();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::OnStart() {
	param_ = initialPram_;

	timer_ = .0f;
	isStop_ = false;
	isSlow_ = false;

	pCameraAnimation_ = pOwner_->GetFollowCamera()->GetCameraAnimation("boostAnimation");
	pCameraAnimation_->CallExecute(true);

	mainAction_ = std::bind(&PlayerActionBoost::BoostCharge, this);

	blur_->Start(param_.bluerStrength, param_.bluerStartTime, true);

	// boostをonにする
	pOwner_->GetJetEngine()->BoostOn();
	pOwner_->GetJetEngine()->JetIsStart();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::OnUpdate() {
	// mainのアクションを実行する
	mainAction_();

	// 中止判定を行なう
	if (CheckStop()) {
		// stopに入る
		mainAction_ = std::bind(&PlayerActionBoost::BoostStop, this);
		blur_->SlowDown(param_.bluerStopTime);
		isSlow_ = true;
	}
	pOwner_->UpdateJoint();

	// ----------------------
	// 向き更新
	// ----------------------
	pOwner_->LookTarget(0.1f, false);

	// ----------------------
	// 追い越し防止処理を行なう
	// ----------------------

	const Math::Vector3 targetPos = pOwner_->GetReticle()->GetTargetPos();
	Math::Vector3 sub = targetPos - pOwnerTransform_->GetPos();
	sub.y = 0;
	float distance = sub.Length();

	if (distance < param_.overTakingThreshold) {
		// stopに入る
		mainAction_ = std::bind(&PlayerActionBoost::BoostStop, this);
		blur_->SlowDown(param_.bluerStopTime);
		isSlow_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::OnEnd() {
	pCameraAnimation_->CallExecute(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次に行うアクションの判定
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::CheckNextAction() {
	if (isStop_) {
		NextAction<PlayerActionMove>();
	}

	if (!isSlow_) {
		if (std::fabs(stick_.x) > param_.toQuickBoostThreshold) {
			AddAction<PlayerActionQuickBoost>();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerActionBoost::IsInput() {
	if (pInput_->IsTriggerButton(XInputButtons::LStickThumb)) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Boost開始前の溜める時間
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::BoostCharge() {
	timer_ += AOENGINE::GameTimer::DeltaTime();

	if (timer_ >= param_.chargeTime) {
		mainAction_ = std::bind(&PlayerActionBoost::Boost, this);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実際にブーストを行なう処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::Boost() {
	direction_ = pOwner_->GetFollowCamera()->GetAngleX().MakeForward();
	acceleration_ = direction_ * param_.boostForce;

	pRigidbody_->AddVelocity(acceleration_);

	// エネルギーを消費する
	pOwner_->ConsumeEN(param_.consumeEnergy * AOENGINE::GameTimer::DeltaTime());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ ブーストの中止
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::BoostStop() {
	Math::Vector3 velocity = pRigidbody_->GetVelocity();
	velocity *= std::pow(param_.stopForce, AOENGINE::GameTimer::DeltaTime());
	pRigidbody_->SetVelocity(velocity);
	if (velocity.Length() < param_.stopThreshold) {
		isStop_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 中止確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerActionBoost::CheckStop() {
	stick_ = AOENGINE::Input::GetInstance()->GetLeftJoyStick(kDeadZone_).Normalize();
	if (stick_.y < -param_.stopThreshold) {
		return true;
	}

	if (pInput_->IsTriggerButton(XInputButtons::ButtonB)) {
		return true;
	}

	if (pOwner_->GetParam().energy <= 0.0f) {
		return true;
	}

	return false;
}
