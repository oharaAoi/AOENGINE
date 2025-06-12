#include "PlayerActionMove.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
#include "Game/Actor/Player/Action/PlayerActionJump.h"
#include "Game/Actor/Player/Action/PlayerActionQuickBoost.h"
#include "Game/Actor/Player/Action/PlayerActionBoost.h"
#include "Game/Actor/Player/Action/PlayerActionShotRight.h"
#include "Game/Actor/Player/Action/PlayerActionShotLeft.h"
// Engine
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Json/JsonItems.h"

#ifdef _DEBUG
void PlayerActionMove::Debug_Gui() {
	ImGui::DragFloat("speed", &parameter_.speed, 0.1f);
	ImGui::DragFloat("boostSpeed", &parameter_.boostSpeed, 0.1f);
	if (ImGui::Button("Save")) {
		JsonItems::Save("PlayerAction", parameter_.ToJson("ActionMove"));
	}
	if (ImGui::Button("Apply")) {
		parameter_.FromJson(JsonItems::GetData("PlayerAction", "ActionMove"));
	}
}
#endif // _DEBUG

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::Build() {
	SetName("actionMove");
	parameter_.FromJson(JsonItems::GetData("PlayerAction", "ActionMove"));
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
	stick_ = Input::GetInstance()->GetLeftJoyStick(kDeadZone_);
	if (stick_.x != 0.0f || stick_.y != 0.0f) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ main action
///////////////////////////////////////////////////////////////////////////////////////////////
 
void PlayerActionMove::Move() {
	stick_ = Input::GetInstance()->GetLeftJoyStick(kDeadZone_).Normalize();

	WorldTransform* transform = pOwner_->GetTransform();
	Vector3 velocity = pOwner_->GetFollowCamera()->GetAngleX().Rotate(Vector3{ stick_.x, 0.0f, stick_.y });
	
	if (pOwner_->GetIsBoostMode()) {
		transform->translate_ += velocity * parameter_.boostSpeed * GameTimer::DeltaTime();
	} else {
		transform->translate_ += velocity * parameter_.speed * GameTimer::DeltaTime();
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
}
