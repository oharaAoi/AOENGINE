#include "PlayerActionIdle.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionMove.h"
#include "Game/Actor/Player/Action/PlayerActionJump.h"
#include "Game/Actor/Player/Action/PlayerActionShotRight.h"
#include "Game/Actor/Player/Action/PlayerActionShotLeft.h"
#include "Game/Actor/Player/Action/PlayerActionRightShoulder.h"
#include "Game/Actor/Player/Action/PlayerActionLeftShoulder.h"
#include "Game/Actor/Player/Action/PlayerActionDeployArmor.h"
#include "Game/Actor/Player/Action/PlayerActionBoost.h"
#include "Game/Actor/Player/Action/PlayerActionQuickBoost.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionIdle::Build() {
	SetName("actionIdle");
	pOwner_->GetGameObject()->GetAnimetor()->TransitionAnimation("idle", 0.2f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionIdle::OnStart() {
	actionTimer_ = 0;

	pOwner_->GetJetEngine()->JetIsStop();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionIdle::OnUpdate() {
	actionTimer_ += GameTimer::DeltaTime();

	pOwner_->RecoveryEN(actionTimer_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionIdle::OnEnd() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次に行うアクションの判定
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionIdle::CheckNextAction() {
	if (CheckInput<PlayerActionBoost>()) {
		NextAction<PlayerActionBoost>();
	}

	if (CheckInput<PlayerActionQuickBoost>()) {
		NextAction<PlayerActionQuickBoost>();
	}

	if (CheckInput<PlayerActionMove>()) {
		NextAction<PlayerActionMove>();
	}

	if (CheckInput<PlayerActionJump>()) {
		NextAction<PlayerActionJump>();
	}

	if (CheckInput<PlayerActionShotRight>()) {
		NextAction<PlayerActionShotRight>();
	}

	if (CheckInput<PlayerActionRightShoulder>()) {
		AddAction<PlayerActionRightShoulder>();
	}

	if (CheckInput<PlayerActionLeftShoulder>()) {
		AddAction<PlayerActionLeftShoulder>();
	}

	if (CheckInput<PlayerActionShotLeft>()) {
		NextAction<PlayerActionShotLeft>();
	}

	if (CheckInput<PlayerActionDeployArmor>()) {
		NextAction<PlayerActionDeployArmor>();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerActionIdle::IsInput() {
	return false;
}
