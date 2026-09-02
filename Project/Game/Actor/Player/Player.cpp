#include "Player.h"

#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Lib/GameTimer.h"

using namespace AOENGINE;

///////////////////////////////////////////////////////////////////////////////////////////////
//  初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Init(BaseGameObject* body) {

	Bind(body);

	// 保存済みの調整値を読み込む
	parameter_.Load();

	velocity_ = CVector3::ZERO;
	facing_ = 1.0f;

	if (WorldTransform* transform = GetTransform()) {
		// シーンに配置された初期位置を開始地点にする
		position_ = transform->GetTranslate();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Update() {
	if (!IsValid()) {
		return;
	}

	const float deltaTime = GameTimer::DeltaTime();

	// 入力をサンプリング
	input_.Update(parameter_.stickDeadZone);

	// 移動更新
	UpdateMove();

	// ジャンプパラメータセット
	const PlayerJump::Params jumpParams{
		parameter_.jumpPower,
		parameter_.hangTime,
		parameter_.riseGravity,
		parameter_.fallGravity,
		parameter_.maxFallSpeed,
	};

	// ジャンプ処理
	jump_.Update(deltaTime, input_.IsJumpTriggered(), jumpParams);
	velocity_.y = jump_.GetVelocityY();

	// 速度 → 位置
	position_ += velocity_ * deltaTime;

	// 仮の地面との着地判定
	ResolveGround();

	// 位置を本体へ反映
	ApplyToBody();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  左右移動
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::UpdateMove() {
	const float horizontal = input_.GetHorizontal();

	velocity_.x = horizontal * parameter_.moveSpeed;
	velocity_.z = 0.0f; // 横スクロールなのでZは固定

	if (horizontal > parameter_.moveInputThreshold) {
		facing_ = 1.0f;
	} else if (horizontal < -parameter_.moveInputThreshold) {
		facing_ = -1.0f;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  仮の地面判定
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::ResolveGround() {
	if (position_.y <= parameter_.groundHeight) {
		position_.y = parameter_.groundHeight;
		jump_.Land();
		velocity_.y = jump_.GetVelocityY();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  本体へ反映
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::ApplyToBody() {
	WorldTransform* transform = GetTransform();
	if (!transform) {
		return;
	}
	transform->SetTranslate(position_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  デバッグ表示
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Debug_Gui() {
	const char* bodyState = "null";
	if (IsValid()) {
		bodyState = "resolved";
	}

	ImGui::Text("body: %s", bodyState);
	ImGui::Text("jumpState: %s", jump_.GetStateName().c_str());
	ImGui::DragFloat3("position", &position_.x, 0.1f);
	ImGui::DragFloat3("velocity", &velocity_.x, 0.1f);

	if (ImGui::CollapsingHeader("Parameter")) {
		// 調整 + Save/Load
		parameter_.Debug_Gui();
	}
}
