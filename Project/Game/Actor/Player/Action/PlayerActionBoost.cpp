#include "PlayerActionBoost.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionMove.h"
#include "Engine/Lib/Json/JsonItems.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::Debug_Gui() {
	ImGui::DragFloat("chargeTime", &initialPram_.chargeTime, .1f);
	ImGui::DragFloat("chargeForce", &initialPram_.chargeForce, .1f);
	ImGui::DragFloat("boostForce", &initialPram_.boostForce, .1f);
	ImGui::DragFloat("stopForce", &initialPram_.stopForce, .1f);

	if (ImGui::Button("Save")) {
		JsonItems::Save(pManager_->GetName(), initialPram_.ToJson(initialPram_.GetName()));
	}
	if (ImGui::Button("Apply")) {
		initialPram_.FromJson(JsonItems::GetData(pManager_->GetName(), initialPram_.GetName()));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

PlayerActionBoost::~PlayerActionBoost() {
	blur_.reset();
}

void PlayerActionBoost::Build() {
	SetName("actionBoost");
	pInput_ = Input::GetInstance();
	pOwnerTransform_ = pOwner_->GetTransform();

	initialPram_.FromJson(JsonItems::GetData(pManager_->GetName(), initialPram_.GetName()));

	blur_ = Engine::GetPostProcess()->GetRadialBlur();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::OnStart() {
	param_ = initialPram_;

	stick_ = pInput_->GetLeftJoyStick();
	direction_ = pOwner_->GetFollowCamera()->GetAngleX().Rotate(Vector3{ stick_.x, 0.0f, stick_.y });

	acceleration_ = direction_ * param_.boostForce;
	velocity_ = { 0.0f, 0.0f, 0.0f };

	timer_ = .0f;
	finishBoost_ = false;

	mainAction_ = std::bind(&PlayerActionBoost::BoostCharge, this);
	pOwner_->GetJetEngine()->JetIsStart();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::OnUpdate() {

	mainAction_();

	if (!finishBoost_) {
		if (CheckStop()) {
			mainAction_ = std::bind(&PlayerActionBoost::BoostStop, this);
			finishBoost_ = true;

			blur_->Stop(0.5f);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::OnEnd() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次に行うアクションの判定
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::CheckNextAction() {
	if (finishBoost_) {
		if (velocity_.Length() < 0.1f) {
			NextAction<PlayerActionMove>();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerActionBoost::IsInput() {
	if (pInput_->GetIsPadTrigger(XInputButtons::LSTICK_THUMB)) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ main action
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::Boost() {
	direction_ = pOwner_->GetFollowCamera()->GetAngleX().MakeForward();
	acceleration_ = direction_ * param_.boostForce;

	velocity_ += acceleration_ * GameTimer::DeltaTime();
	pOwnerTransform_->translate_ += velocity_;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ boostCharge
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::BoostCharge() {
	timer_ += GameTimer::DeltaTime();

	if (timer_ < param_.chargeTime) {
		Vector3 backDirection = pOwner_->GetTransform()->rotation_.MakeForward();
		acceleration_ = (backDirection.Normalize() * -1.0f) * param_.boostForce;
		velocity_ += acceleration_ * GameTimer::DeltaTime();
		pOwnerTransform_->translate_ += velocity_;
	} else {
		mainAction_ = std::bind(&PlayerActionBoost::Boost, this);
		blur_->Start(1.f, 2.0f);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ ブーストの中止
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionBoost::BoostStop() {
	velocity_ *= param_.stopForce;
	pOwnerTransform_->translate_ += velocity_;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 中止確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerActionBoost::CheckStop() {
	stick_ = Input::GetInstance()->GetLeftJoyStick(kDeadZone_).Normalize();
	if (stick_.y < -0.1f) {
		return true;
	}

	if (pInput_->GetIsPadTrigger(XInputButtons::BUTTON_B)) {
		return true;
	}

	return false;
}
