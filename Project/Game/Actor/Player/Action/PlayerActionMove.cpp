#include "PlayerActionMove.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
#include "Game/Actor/Player/Action/PlayerActionJump.h"
#include "Game/Actor/Player/Action/PlayerActionQuickBoost.h"
#include "Game/Actor/Player/Action/PlayerActionBoost.h"
#include "Game/Actor/Player/Action/PlayerActionShotRight.h"
#include "Game/Actor/Player/Action/PlayerActionShotLeft.h"
#include "Game/Actor/Player/Action/PlayerActionTurnAround.h"
// Engine
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Json/JsonItems.h"

#ifdef _DEBUG
void PlayerActionMove::Debug_Gui() {
	ImGui::DragFloat("speed", &param_.speed, 0.1f);
	ImGui::DragFloat("boostSpeed", &param_.boostSpeed, 0.1f);
	if (ImGui::Button("Save")) {
		JsonItems::Save("PlayerAction", param_.ToJson("ActionMove"));
	}
	if (ImGui::Button("Apply")) {
		param_.FromJson(JsonItems::GetData("PlayerAction", "ActionMove"));
	}
}
#endif // _DEBUG

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::Build() {
	SetName("actionMove");
	initParam_.FromJson(JsonItems::GetData("PlayerAction", "ActionMove"));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::OnStart() {
	actionTimer_ = 0;

	if (pOwner_->GetIsBoostMode() || !pOwner_->GetIsLanding()) {
		pOwner_->GetJetEngine()->JetIsStart();
	} else {
		pOwner_->GetJetEngine()->JetIsStop();
	}

	pOwner_->SetIsMoving(true);

	param_ = initParam_;

	isTurnAround_ = false;

	Vector2 current = Input::GetInstance()->GetLeftJoyStick(kDeadZone_).Normalize();
	preVelocity_ = Vector3(current.x, 0.0f, current.y);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::OnUpdate() {
	actionTimer_ += GameTimer::DeltaTime();

	Move();

	// 移動の入力がなかったら回復処理を行っていく
	if (pOwner_->GetIsLanding()) {
		pOwner_->RecoveryEN(actionTimer_);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::OnEnd() {
	pOwner_->SetIsMoving(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次に行うアクションの判定
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::CheckNextAction() {
	if (!IsInput()) {
		if (pOwner_->GetIsLanding()) {
			NextAction<PlayerActionIdle>();
		}
	}

	if (isTurnAround_) {
		// 反対を向く処理をす
		if (pOwner_->GetIsBoostMode()) {
			context_->Set<float>("speed", param_.boostSpeed);
		} else {
			context_->Set<float>("speed", param_.speed);
		}
		NextAction<PlayerActionTurnAround>();
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

	if (CheckInput<PlayerActionQuickBoost>()) {
		NextAction<PlayerActionQuickBoost>();
	}

	if (CheckInput<PlayerActionBoost>()) {
		NextAction<PlayerActionBoost>();
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerActionMove::IsInput() {
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
	inputStick_ = Input::GetInstance()->GetLeftJoyStick(kDeadZone_).Normalize();

	WorldTransform* transform = pOwner_->GetTransform();
	Vector3 velocity = pOwner_->GetFollowCamera()->GetAngleX().Rotate(Vector3{ inputStick_.x, 0.0f, inputStick_.y });

	// 方向転換をしなかったら
	if (IsDirectionReversed(velocity)) { 
		preVelocity_ = velocity;
		//isTurnAround_ = true;
		return;
	}

	// 減速処理
	Deceleration(inputStick_);

	if (pOwner_->GetIsBoostMode()) {
		transform->translate_ += velocity * param_.boostSpeed * GameTimer::DeltaTime();
	} else {
		transform->translate_ += velocity * param_.speed * GameTimer::DeltaTime();
	}

	// playerを移動方向に向ける
	if (velocity.x != 0.0f || velocity.y != 0.0f) {
		float angle = std::atan2f(velocity.x, velocity.z);
		Quaternion lerpQuaternion = Quaternion::Slerp(transform->rotation_, Quaternion::AngleAxis(angle, CVector3::UP), 0.1f);
		transform->rotation_ = lerpQuaternion;
	}

	if (pOwner_->GetIsBoostMode() || !pOwner_->GetIsLanding()) {
		pOwner_->GetJetEngine()->JetIsStart();
	} else {
		pOwner_->GetJetEngine()->JetIsStop();
	}

	preVelocity_ = velocity;
}

void PlayerActionMove::Deceleration(Vector2 currentInput) {
	if (currentInput.Length() <= 0.0f) {
		if (pOwner_->GetIsBoostMode()) {
			param_.boostSpeed -= 2.f * GameTimer::DeltaTime();
			param_.boostSpeed = std::clamp(param_.boostSpeed, 0.0f, initParam_.boostSpeed);
		} else {
			param_.speed -= 2.f * GameTimer::DeltaTime();
			param_.speed = std::clamp(param_.speed, 0.0f, initParam_.speed);
		}
	} else {
		param_.boostSpeed = initParam_.boostSpeed;
		param_.speed = initParam_.speed;
	}
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
