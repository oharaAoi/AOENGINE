#include "PlayerActionJump.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
#include "Game/Actor/Player/Action/PlayerActionMove.h"
// Engine
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Json/JsonItems.h"

#ifdef _DEBUG
void PlayerActionJump::Debug_Gui() {
	ImGui::DragFloat("smallJumpTime", &smallJumpTime_, 0.1f);

	ImGui::DragFloat("jumpForce", &param_.jumpForce, 0.1f);
	ImGui::DragFloat("risingForce", &param_.risingForce, 0.1f);
	ImGui::DragFloat("maxAcceleration", &param_.maxAcceleration, 0.1f);
	ImGui::DragFloat("jumpEnergy", &param_.jumpEnergy, 0.1f);
	ImGui::DragFloat("cameraShakeTime", &param_.cameraShakeTime, 0.1f);
	ImGui::DragFloat("cameraShakeStrength", &param_.cameraShakeStrength, 0.1f);
	
	if (ImGui::Button("Save")) {
		JsonItems::Save("PlayerAction", param_.ToJson("ActionJump"));
	}
	if (ImGui::Button("Apply")) {
		param_.FromJson(JsonItems::GetData("PlayerAction", "ActionJump"));
	}
	ImGui::Text("acceleration: (%.2f, %.2f, %.2f)", acceleration_.x, acceleration_.y, acceleration_.z);
	ImGui::Text("velocity: (%.2f, %.2f, %.2f)", velocity_.x, velocity_.y, velocity_.z);
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::Build() {
	SetName("actionJump");
	pOwnerTransform_ = pOwner_->GetTransform();
	param_.FromJson(JsonItems::GetData("PlayerAction", "ActionJump"));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::OnStart() {
	actionTimer_ = 0.0f;
	isFall_ = false;
	
	// ジャンプした分のエネルギーを消費しておく
	pOwner_->ConsumeEN(param_.jumpEnergy);

	mainAction_ = std::bind(&PlayerActionJump::Jump, this);
	isJump_ = false;

	pOwner_->GetFollowCamera()->SetShake(param_.cameraShakeTime, param_.cameraShakeStrength);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::OnUpdate() {
	actionTimer_ += GameTimer::DeltaTime();

	mainAction_();
	// 上昇をする
	Rising();
	// 重力を適用させる
	ApplyGravity();
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
	/*if (pOwnerTransform_->translate_.y <= 0.0f) {
		pOwnerTransform_->translate_.y = 0.0f;
		NextAction<PlayerActionIdle>();
	}*/

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
	if (isJump_) { return; }

	velocity_.y = param_.jumpForce;
	acceleration_.y = param_.jumpForce * GameTimer::DeltaTime();
	pOwner_->SetIsLanding(false);

	isJump_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 上昇を行う
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::Rising() {
	if (!isFall_) { return; }

	// ボタンを押していたら上昇する
	if (Input::GetInstance()->GetPressPadTrigger(XInputButtons::BUTTON_A)) {
		isRising_ = true;
	} else {
		isRising_ = false;
	}

	// 上昇していたなら
	if (isRising_) {
		acceleration_.y += param_.risingForce * GameTimer::DeltaTime();
		// エネルギーを消費する
		pOwner_->ConsumeEN(param_.jumpEnergy * GameTimer::DeltaTime());

		if (velocity_.y <= 0.0f) {
			acceleration_.y = 0.0f;
			velocity_.y = 4.0f;
		}
	}

	// 上昇をやめたら速度を0.0fに近い値にする
	if (!isRising_ && isPreRising_) {
		velocity_.y = std::lerp(velocity_.y, 0.0f, 0.8f);
	}

	isPreRising_ = isRising_;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 重力の適用
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionJump::ApplyGravity() {
	acceleration_.y += (kGravity * pOwner_->GetParam().bodyWeight) * GameTimer::DeltaTime();
	acceleration_.y = std::clamp(acceleration_.y, -param_.maxAcceleration, param_.maxAcceleration);

	velocity_ += acceleration_ * GameTimer::DeltaTime();
	pOwnerTransform_->translate_ += velocity_ * GameTimer::DeltaTime();

	if (velocity_.y <= 0.0f) {
		isFall_ = true;
	}
}
