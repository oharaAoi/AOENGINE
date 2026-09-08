#include "StopperToBlockGroupCallBacks.h"

/// engine
#include "Engine/Core/Engine.h"
#include "Engine/Lib/GameTimer.h"

/// game
#include "Game/Stage/BlockGroupLauncherManager.h"

void StopperToBlockGroupCallBacks::Init() {
	SetCallBacks();

	// 集合中のブロックは "Block"、打ち上げ中は "LaunchedBlock" になるため、どちらでも受け取る
	SetPair(pCollisionManager_, kStopperName_, kBlockName_);
	SetPair(pCollisionManager_, kStopperName_, kLaunchedBlockName_);
}

void StopperToBlockGroupCallBacks::CollisionEnter(AOENGINE::BaseCollider* const, AOENGINE::BaseCollider* const block) {
	if (!block || !pLauncherManager_) {
		return;
	}

	// 足場として置かれているだけのブロック(足止めが乗っているブロックなど)では鳴らさない。
	// ランチャーが動かしている塊に属しているブロックだけを当たったことにする
	if (pLauncherManager_->GetBlockCountByCollider(block) <= 0) {
		return;
	}

	// 塊のブロックが次々に当たるうえ、同じ衝突でも enter が複数回呼ばれるため、
	// 短い間隔で重ねて鳴らさないようにする
	const float now = AOENGINE::GameTimer::TotalTime();
	if (now - lastPlayedTime_ < kPlayInterval_) {
		return;
	}
	lastPlayedTime_ = now;

	// se
	Engine::GetSoundManager()->Play(kSeName_);
}

void StopperToBlockGroupCallBacks::CollisionStay(AOENGINE::BaseCollider* const, AOENGINE::BaseCollider* const) {
	// 触れ続けている間は鳴らさない
}

void StopperToBlockGroupCallBacks::CollisionExit(AOENGINE::BaseCollider* const, AOENGINE::BaseCollider* const) {
	// 離れた時は何もしない
}
