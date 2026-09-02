#include "Player.h"

#include <algorithm>

#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/System/Input/Input.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Lib/GameTimer.h"

using namespace AOENGINE;

Player::Player() {
	object_ = nullptr;
	transform_ = nullptr;
	SetName("Player");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Init(BaseGameObject* body) {
	object_ = body;
	transform_ = nullptr;
	if (body) {
		transform_ = body->GetTransform();
	}

	velocity_ = CVector3::ZERO;
	jumpState_ = JumpState::Grounded;
	hangTimer_ = 0.0f;
	isLaunching_ = false;
	launchTimer_ = 0.0f;
	facing_ = 1.0f;

	if (transform_) {
		// シーンに配置された初期位置を開始地点にする
		position_ = transform_->GetTranslate();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Update() {
	if (!object_ || !transform_) {
		return;
	}

	const float deltaTime = GameTimer::DeltaTime();

	// 入力 → 速度
	InputMove();
	UpdateLaunch(deltaTime);
	UpdateJump(deltaTime);

	// 速度 → 位置
	position_ += velocity_ * deltaTime;

	// 仮の地面との着地判定
	ResolveGround();

	// 位置を本体へ反映
	ApplyToBody();
}


void Player::InputMove() {
	const float horizontal = ReadHorizontal();

	velocity_.x = horizontal * parameter_.moveSpeed;
	velocity_.z = 0.0f; // 横スクロールなのでZは固定

	if (horizontal > 0.01f) {
		facing_ = 1.0f;
	} else if (horizontal < -0.01f) {
		facing_ = -1.0f;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 打ち上げ (E ・ パッドBボタン)
//    仕様書では操作のみ規定。ここでは暫定で上方向へ強く打ち上げる。
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::UpdateLaunch(float deltaTime) {
	if (!isLaunching_ && IsLaunchTriggered()) {
		isLaunching_ = true;
		launchTimer_ = 0.0f;

		velocity_.y = parameter_.launchPower;
		jumpState_ = JumpState::Rising;
	}

	if (isLaunching_) {
		launchTimer_ += deltaTime;
		if (launchTimer_ >= parameter_.launchDuration) {
			isLaunching_ = false;
		}
	}
}

void Player::UpdateJump(float deltaTime) {
	switch (jumpState_) {
	case JumpState::Grounded:
		velocity_.y = 0.0f;
		if (IsJumpTriggered()) {
			velocity_.y = parameter_.jumpPower;
			jumpState_ = JumpState::Rising;
		}
		break;

	case JumpState::Rising:
		velocity_.y -= parameter_.riseGravity * deltaTime;
		if (velocity_.y <= 0.0f) {
			// 頂点に到達 → 滞空へ
			velocity_.y = 0.0f;
			hangTimer_ = 0.0f;
			jumpState_ = JumpState::Hanging;
		}
		break;

	case JumpState::Hanging:
		// 滞空中は高さを維持する
		velocity_.y = 0.0f;
		hangTimer_ += deltaTime;
		if (hangTimer_ >= parameter_.hangTime) {
			jumpState_ = JumpState::Falling;
		}
		break;

	case JumpState::Falling:
		velocity_.y -= parameter_.fallGravity * deltaTime;
		if (velocity_.y < -parameter_.maxFallSpeed) {
			velocity_.y = -parameter_.maxFallSpeed;
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 仮の地面判定
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::ResolveGround() {
	if (position_.y <= parameter_.groundHeight) {
		position_.y = parameter_.groundHeight;

		if (velocity_.y < 0.0f) {
			velocity_.y = 0.0f;
		}
		if (jumpState_ == JumpState::Falling || jumpState_ == JumpState::Hanging) {
			jumpState_ = JumpState::Grounded;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 本体へ反映
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::ApplyToBody() {
	transform_->SetTranslate(position_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力ヘルパー
///////////////////////////////////////////////////////////////////////////////////////////////

float Player::ReadHorizontal() const {
	float horizontal = 0.0f;

	if (Input::IsPressKey(DIK_D) || Input::IsPressKey(DIK_RIGHT)) {
		horizontal += 1.0f;
	}
	if (Input::IsPressKey(DIK_A) || Input::IsPressKey(DIK_LEFT)) {
		horizontal -= 1.0f;
	}

	// コントローラー左スティック左右
	const Math::Vector2 stick = Input::GetLeftJoyStick(parameter_.stickDeadZone);
	horizontal += stick.x;

	return std::clamp(horizontal, -1.0f, 1.0f);
}

bool Player::IsJumpTriggered() const {
	return Input::IsTriggerKey(DIK_SPACE) || Input::IsTriggerButton(ButtonA);
}

bool Player::IsLaunchTriggered() const {
	return Input::IsTriggerKey(DIK_E) || Input::IsTriggerButton(ButtonB);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ デバッグ表示
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Debug_Gui() {
	const char* stateName = "Grounded";
	switch (jumpState_) {
	case JumpState::Rising:  stateName = "Rising";  break;
	case JumpState::Hanging: stateName = "Hanging"; break;
	case JumpState::Falling: stateName = "Falling"; break;
	default: break;
	}

	const char* bodyState = "null";
	if (object_) {
		bodyState = "resolved";
	}

	const char* launchState = "false";
	if (isLaunching_) {
		launchState = "true";
	}

	ImGui::Text("body: %s", bodyState);
	ImGui::Text("jumpState: %s", stateName);
	ImGui::Text("launching: %s", launchState);
	ImGui::DragFloat3("position", &position_.x, 0.1f);
	ImGui::DragFloat3("velocity", &velocity_.x, 0.1f);
}
