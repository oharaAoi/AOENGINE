#include "TutorialBehavior.h"
#include "Game/Actor/Player/Player.h"
#include <Game/Actor/Player/Action/PlayerActionMove.h>
#include <Game/Actor/Player/Action/PlayerActionJump.h>
#include <Game/Actor/Player/Action/PlayerActionQuickBoost.h>

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 移動のTutorial
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialMoveBehavior::Init() {
	isNext_ = false;
	totalMoveTime_ = 0;
	host_->tutorialMissionGauge_->Success(false);
	host_->tutorialMissionGauge_->ChangeControlUI("tutorial_move.png");
}

void TutorialMoveBehavior::Update() {
	Player* pPlayer = host_->playerManager_->GetPlayer();
	// 移動のアクションが実行されている間は時間を計測’する
	if (pPlayer->GetActionManager()->ExistAction(typeid(PlayerActionMove).hash_code())) {
		totalMoveTime_ += GameTimer::DeltaTime();
	}

	// Target量を超えたら
	if (!isNext_) {
		if (totalMoveTime_ >= targetValue_) {
			isNext_ = true;
			host_->tutorialMissionGauge_->Success(true);
		}
	}

	// 割合を求めておく
	float ratio = totalMoveTime_ / targetValue_;
	host_->tutorialMissionGauge_->FillAmountGauge(ratio);

	if (isNext_) {
		if (host_->tutorialMissionGauge_->GetIsSuccessFinish()) {
			host_->ChangeBehavior(new TutorialJumpBehavior(host_));
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ jumpのTutorial
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialJumpBehavior::Init() {
	isNext_ = false;
	totalJumpTime_ = 0;
	host_->tutorialMissionGauge_->Success(false);
	host_->tutorialMissionGauge_->ChangeControlUI("tutorial_jump.png");
}

void TutorialJumpBehavior::Update() {
	Player* pPlayer = host_->playerManager_->GetPlayer();
	// 移動のアクションが実行されている間は時間を計測’する
	if (pPlayer->GetActionManager()->ExistAction(typeid(PlayerActionJump).hash_code())) {
		totalJumpTime_ += GameTimer::DeltaTime();
	}

	// Target量を超えたら
	if (!isNext_) {
		if (totalJumpTime_ >= targetValue_) {
			isNext_ = true;
			host_->tutorialMissionGauge_->Success(true);
		}
	}

	// 割合を求めておく
	float ratio = totalJumpTime_ / targetValue_;
	host_->tutorialMissionGauge_->FillAmountGauge(ratio);

	if (isNext_) {
		if (host_->tutorialMissionGauge_->GetIsSuccessFinish()) {
			host_->ChangeBehavior(new TutorialBoostBehavior(host_));
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ BoostのTutotrial
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialBoostBehavior::Init() {
	isNext_ = false;
	totalBoostCount = 0;
	isCount_ = false;
	host_->tutorialMissionGauge_->Success(false);
	host_->tutorialMissionGauge_->ChangeControlUI("tutorial_boost.png");
}

void TutorialBoostBehavior::Update() {
	Player* pPlayer = host_->playerManager_->GetPlayer();
	// 移動のアクションが実行されている間は時間を計測する
	if (!isCount_) {
		if (pPlayer->GetActionManager()->ExistAction(typeid(PlayerActionQuickBoost).hash_code())) {
			totalBoostCount++;
			isCount_ = true;
		}
	} else {
		if (!pPlayer->GetActionManager()->ExistAction(typeid(PlayerActionQuickBoost).hash_code())) {
			isCount_ = false;
		}
	}

	// Target量を超えたら
	if (!isNext_) {
		if (totalBoostCount >= targetValue_) {
			isNext_ = true;
			host_->tutorialMissionGauge_->Success(true);
		}
	}

	// 割合を求めておく
	float ratio = (float)totalBoostCount / (float)targetValue_;
	host_->tutorialMissionGauge_->FillAmountGauge(ratio);

	if (isNext_) {
		if (host_->tutorialMissionGauge_->GetIsSuccessFinish()) {
			host_->ChangeBehavior(new TutorialLockOnBehavior(host_));
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ LockOnのTutorial
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialLockOnBehavior::Init() {
	isNext_ = false;
	totalLockOnTime_ = 0;
	host_->tutorialMissionGauge_->Success(false);
	host_->tutorialMissionGauge_->ChangeControlUI("tutorial_lockOn.png");
}

void TutorialLockOnBehavior::Update() {
	Player* pPlayer = host_->playerManager_->GetPlayer();
	if (pPlayer->GetIsLockOn()) {
		totalLockOnTime_ += GameTimer::DeltaTime();
	}

	if (!isNext_) {
		if (totalLockOnTime_ >= targetValue_) {
			isNext_ = true;
			host_->tutorialMissionGauge_->Success(true);
		}
	}

	// 割合を求めておく
	float ratio = (float)totalLockOnTime_ / (float)targetValue_;
	host_->tutorialMissionGauge_->FillAmountGauge(ratio);

	if (isNext_) {
		if (host_->tutorialMissionGauge_->GetIsSuccessFinish()) {
			host_->ChangeBehavior(new TutorialAttackBehavior(host_));
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 攻撃のTutorial
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialAttackBehavior::Init() {
	isNext_ = false;
	totalAttackCout_ = 0;
	host_->tutorialMissionGauge_->ChangeControlUI("tutorial_attack.png");
}

void TutorialAttackBehavior::Update() {
	Player* pPlayer = host_->playerManager_->GetPlayer();
	// 移動のアクションが実行されている間は時間を計測’する
	if (pPlayer->IsAttack()) {
		totalAttackCout_++;
	}

	if (!isNext_) {
		if (totalAttackCout_ >= targetValue_) {
			isNext_ = true;
			host_->tutorialMissionGauge_->Success(true);
			host_->ToGameScene();
		}
	}

	// 割合を求めておく
	float ratio = (float)totalAttackCout_ / (float)targetValue_;
	host_->tutorialMissionGauge_->FillAmountGauge(ratio);
}
