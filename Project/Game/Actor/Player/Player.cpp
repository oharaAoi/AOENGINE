#include "Player.h"

#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Engine/Module/Components/Collider/BaseCollider.h"
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

	facing_ = 1.0f;

	if (BaseGameObject* object = GetGameObject()) {
		// SceneやPrefabにRigidbodyが無い場合はここで用意する
		if (object->GetRigidbody() == nullptr) {
			object->SetPhysics();
		}

		if (Rigidbody* rigidbody = object->GetRigidbody()) {
			// 上下の動きはPlayerJumpが管理するのでRigidbody側の重力は使わない
			rigidbody->SetGravity(false);
			rigidbody->SetVelocity(CVector3::ZERO);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Update() {
	if (!IsValid()) {
		return;
	}

	Rigidbody* rigidbody = GetRigidbody();
	if (!rigidbody) {
		return;
	}

	const float deltaTime = GameTimer::DeltaTime();

	// 速度から位置への反映も押し戻しもBaseGameObject側で行われるため、
	// 前フレームの結果に対して先に接地判定を行う
	ResolveGround();

	// 入力をサンプリング
	input_.Update(parameter_.stickDeadZone);

	// 移動更新
	UpdateMove(rigidbody);

	// ジャンプパラメータセット
	const PlayerJump::Params jumpParams{
		parameter_.jumpPower,
		parameter_.hangTime,
		parameter_.riseGravity,
		parameter_.fallGravity,
		parameter_.maxFallSpeed,
		parameter_.groundKeepSpeed,
	};

	// ジャンプ処理
	jump_.Update(deltaTime, input_.IsJumpTriggered(), jumpParams);
	rigidbody->SetVelocityY(jump_.GetVelocityY());

	// 先に前フレームまでの接続受付時間を消化してから、今フレームのジャンプで新しい受付を開始する
	blockGroupConnectState_.Update(deltaTime);
	if (jump_.IsJumpStarted()) {
		blockGroupConnectState_.Begin(parameter_.connectableTime);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  Rigidbodyの取得
///////////////////////////////////////////////////////////////////////////////////////////////

Rigidbody* Player::GetRigidbody() const {
	BaseGameObject* object = GetGameObject();
	if (!object) {
		return nullptr;
	}
	return object->GetRigidbody();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  左右移動
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::UpdateMove(Rigidbody* rigidbody) {
	const float horizontal = input_.GetHorizontal();

	rigidbody->SetVelocityX(horizontal * parameter_.moveSpeed);
	rigidbody->SetVelocityZ(0.0f); // 横スクロールなのでZは固定

	if (horizontal > parameter_.moveInputThreshold) {
		facing_ = 1.0f;
	} else if (horizontal < -parameter_.moveInputThreshold) {
		facing_ = -1.0f;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  接地判定
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::ResolveGround() {
	bool isSupported = false;

	// 仮の地面へのめり込みを戻す
	if (WorldTransform* transform = GetTransform()) {
		Math::Vector3 translate = transform->GetTranslate();
		if (translate.y <= parameter_.groundHeight) {
			translate.y = parameter_.groundHeight;
			transform->SetTranslate(translate);
			isSupported = true;
		}
	}

	// Blockからの押し戻しで足場に乗ったかを見る
	if (ResolvePushback()) {
		isSupported = true;
	}

	if (isSupported) {
		jump_.Land();
	} else {
		// 足場から外れたら落下させる
		jump_.LeaveGround();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  押し戻しの反映
///////////////////////////////////////////////////////////////////////////////////////////////

bool Player::ResolvePushback() {
	// 押し戻し量はCollisionManagerが求め、座標へはBaseGameObject::PostUpdate()が反映している。
	// ここでは前フレームに押し戻された向きを見てジャンプ状態だけを合わせる。
	BaseCollider* collider = GetCollider(kColliderTag);
	if (!collider) {
		return false;
	}

	const Math::Vector3& pushback = collider->GetPushBackDirection();

	if (pushback.y > kPushbackThreshold) {
		// 下から押し戻された = 足場の上に乗っている
		return true;
	}

	if (pushback.y < -kPushbackThreshold) {
		// 上から押し戻された = 頭をぶつけた
		jump_.HitCeiling();
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  速度の取得
///////////////////////////////////////////////////////////////////////////////////////////////

Math::Vector3 Player::GetVelocity() const {
	const Rigidbody* rigidbody = GetRigidbody();
	if (!rigidbody) {
		return CVector3::ZERO;
	}
	return rigidbody->GetVelocity();
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
	ImGui::Text("rigidbody: %s", GetRigidbody() != nullptr ? "resolved" : "null");
	ImGui::Text("jumpState: %s", jump_.GetStateName().c_str());
	ImGui::Text("connectRemain: %.2f", blockGroupConnectState_.GetRemainingTime());
	ImGui::Text("connectGroups: %d", static_cast<int>(blockGroupConnectState_.GetGroupIds().size()));

	if (WorldTransform* transform = GetTransform()) {
		Math::Vector3 position = transform->GetTranslate();
		if (ImGui::DragFloat3("position", &position.x, 0.1f)) {
			transform->SetTranslate(position);
		}
	}

	if (Rigidbody* rigidbody = GetRigidbody()) {
		Math::Vector3 velocity = rigidbody->GetVelocity();
		if (ImGui::DragFloat3("velocity", &velocity.x, 0.1f)) {
			rigidbody->SetVelocity(velocity);
		}
	}

	if (ImGui::CollapsingHeader("Parameter")) {
		// 調整 + Save/Load
		parameter_.Debug_Gui();
	}
}
