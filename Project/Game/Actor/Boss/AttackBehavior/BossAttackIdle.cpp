#include "BossAttackIdle.h"

#include "Engine/Lib/Math/MyRandom.h"

#include "Game/Actor/Boss/Boss.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//  開始
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackIdle::Enter(Boss& boss) {

	const BossParameter& param = boss.GetParameter();

	// min/maxが逆に設定されても壊れないように、小さい方を先にしておく
	float minTime = param.idleTimeMin;
	float maxTime = param.idleTimeMax;
	if (minTime > maxTime) {
		const float swapped = minTime;
		minTime = maxTime;
		maxTime = swapped;
	}

	// 一定間隔だと動きが読まれてしまうので、min~maxの間でばらつかせる
	remainingTime_ = Random::RandomFloat(minTime, maxTime);

	isFinished_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackIdle::Update(Boss& boss, float deltaTime) {
	(void)boss;

	// 待つだけ。時間が来たら次の行動へ渡す
	remainingTime_ -= deltaTime;
	if (remainingTime_ <= 0.0f) {
		isFinished_ = true;
	}
}
