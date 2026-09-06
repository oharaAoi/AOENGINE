#include "Game/Tutorial/Step/TutorialStepConnect.h"
#include "Game/Tutorial/Step/TutorialStepDamageFloor.h"
#include "Game/Tutorial/Step/TutorialStepEnding.h"
#include "Game/Tutorial/Step/TutorialStepLaunch.h"
#include "Game/Tutorial/Step/TutorialStepMove.h"

#include <cmath>

#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Tutorial/TutorialContext.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//  移動とジャンプ
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialStepMove::Enter(TutorialContext& context) {
	(void)context;

	hasMoved_ = false;
	hasJumped_ = false;
}

void TutorialStepMove::Update(TutorialContext& context, float deltaTime) {
	(void)deltaTime;

	const Player* player = context.GetPlayer();
	if (player == nullptr) {
		return;
	}

	// 横に動いたか
	if (std::abs(player->GetVelocity().x) > kMoveSpeedThreshold) {
		hasMoved_ = true;
	}

	// 接地していなければジャンプしたとみなす
	if (!player->IsGrounded()) {
		hasJumped_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ダメージ床
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialStepDamageFloor::Enter(TutorialContext& context) {

	healTimer_ = 0.0f;
	isWaitingHeal_ = false;
	hasLaunched_ = false;

	// 何度でも試せるように、始める時点で満タンにしておく
	if (Player* player = context.GetPlayer()) {
		player->HealFull();
	}
}

void TutorialStepDamageFloor::Update(TutorialContext& context, float deltaTime) {

	// ダメージ床で打ち上げられたら達成
	if (const Player* player = context.GetPlayer()) {
		if (player->IsDamageFloorAirborne()) {
			hasLaunched_ = true;
		}
	}

	UpdateHealRecover(context, deltaTime);
}

void TutorialStepDamageFloor::Exit(TutorialContext& context) {

	// 次のページへ減ったまま持ち越さない
	if (Player* player = context.GetPlayer()) {
		player->HealFull();
	}
}

void TutorialStepDamageFloor::UpdateHealRecover(TutorialContext& context, float deltaTime) {

	Player* player = context.GetPlayer();
	if (player == nullptr) {
		return;
	}

	// 満タンなら数え直すだけ
	if (player->GetCurrentHp() >= player->GetMaxHp()) {
		healTimer_ = 0.0f;
		isWaitingHeal_ = false;
		return;
	}

	// 減った瞬間から待ち時間を数え始める
	if (!isWaitingHeal_) {
		isWaitingHeal_ = true;
		healTimer_ = 0.0f;
	}

	healTimer_ += deltaTime;
	if (healTimer_ < kHealWaitTime) {
		return;
	}

	// ちょっと待ってから満タンへ戻す
	player->HealFull();
	isWaitingHeal_ = false;
	healTimer_ = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ブロックをつなぐ
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialStepConnect::Enter(TutorialContext& context) {
	(void)context;

	hasConnected_ = false;
}

void TutorialStepConnect::Update(TutorialContext& context, float deltaTime) {
	(void)deltaTime;

	const Player* player = context.GetPlayer();
	if (player == nullptr) {
		return;
	}

	// 一度でもグループをつないだら達成
	if (player->GetConnectedGroupCount() > 0) {
		hasConnected_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  打ち上げ
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialStepLaunch::Enter(TutorialContext& context) {

	isFinished_ = false;
	startBossHp_ = 0.0f;

	// 当たったかどうかは、HPが減ったかで見る
	if (const Boss* boss = context.GetBoss()) {
		startBossHp_ = boss->GetCurrentHp();
	}
}

void TutorialStepLaunch::Update(TutorialContext& context, float deltaTime) {
	(void)deltaTime;

	const Boss* boss = context.GetBoss();
	if (boss == nullptr) {
		return;
	}

	// ボスに当てたら次のページへ
	if (boss->GetCurrentHp() < startBossHp_) {
		isFinished_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  タイトルに戻る
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialStepEnding::Update(TutorialContext& context, float deltaTime) {
	(void)context;
	(void)deltaTime;

	// 送る入力を待つだけ。実際の遷移はシーン側が行う
}
