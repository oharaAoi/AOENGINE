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

PlayerActionMove::~PlayerActionMove() {
}

void PlayerActionMove::Build() {
	SetName("actionMove");
	parameter_.FromJson(JsonItems::GetData("PlayerAction", "ActionMove"));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::OnStart() {
	jumpAction_ = pManager_->GetActionInstance<PlayerActionJump>();
	shotActionRight_ = pManager_->GetActionInstance<PlayerActionShotRight>();
	shotActionLeft_ = pManager_->GetActionInstance<PlayerActionShotLeft>();
	quickBoostAction_ = pManager_->GetActionInstance<PlayerActionQuickBoost>();
	boostAction_ = pManager_->GetActionInstance<PlayerActionBoost>();

	actionTimer_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::OnUpdate() {
	actionTimer_ += GameTimer::DeltaTime();
	pOwner_->RecoveryEN(actionTimer_);

	Move();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::OnEnd() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次に行うアクションの判定
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::CheckNextAction() {
	if (!IsInput()) {
		NextAction<PlayerActionIdle>();
	}

	if (jumpAction_->IsInput()) {
		AddAction<PlayerActionJump>();
	}

	if (shotActionRight_->IsInput()) {
		AddAction<PlayerActionShotRight>();
	}

	if (shotActionLeft_->IsInput()) {
		AddAction<PlayerActionShotLeft>();
	}

	if (quickBoostAction_->IsInput()) {
		NextAction<PlayerActionQuickBoost>();
	}

	if (boostAction_->IsInput()) {
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
	
	transform->translate_ += velocity * parameter_.speed * GameTimer::DeltaTime();

	// playerを移動方向に向ける
	if (velocity.x != 0.0f || velocity.y != 0.0f) {
		float angle = std::atan2f(velocity.x, velocity.z);
		Quaternion lerpQuaternion = Quaternion::Slerp(transform->rotation_, Quaternion::AngleAxis(angle, CVector3::UP), 0.1f);
		transform->rotation_ = lerpQuaternion;
	}
}
