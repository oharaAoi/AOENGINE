#include "BossPhaseChange.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include "Game/Actor/Boss/Boss.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//  開始
///////////////////////////////////////////////////////////////////////////////////////////////

void BossPhaseChange::Enter(Boss& boss) {

	elapsedTime_ = 0.0f;
	isFinished_ = false;

	// 切り替えの最中に殴られて素通りされないようにする
	boss.SetInvincible(true);

	// 合図としてカメラを揺らす
	boss.ShakeCamera(boss.GetParameter().phaseChangeShake);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void BossPhaseChange::Update(Boss& boss, float deltaTime) {

	const float duration = boss.GetParameter().phaseChangeTime;

	elapsedTime_ += deltaTime;

	// 0.0 -> 1.0 の進み具合。時間が0でも壊れないようにしておく
	float ratio = 1.0f;
	if (duration > 0.0f) {
		ratio = std::clamp(elapsedTime_ / duration, 0.0f, 1.0f);
	}

	UpdateScalePulse(boss, ratio);

	if (ratio >= 1.0f) {
		isFinished_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  終了
///////////////////////////////////////////////////////////////////////////////////////////////

void BossPhaseChange::Exit(Boss& boss) {

	// 途中で切り替わった場合も、必ず元の大きさと状態へ戻す
	boss.SetScaleMultiplier(CVector3::UNIT);
	boss.SetInvincible(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  膨らんで戻る動き
///////////////////////////////////////////////////////////////////////////////////////////////

void BossPhaseChange::UpdateScalePulse(Boss& boss, float ratio) const {

	// sinは0->1->0と動くので、始めと終わりで必ず元の大きさに戻る
	const float pulse = std::sin(ratio * std::numbers::pi_v<float>);
	const float rate = 1.0f + pulse * boss.GetParameter().phaseChangeScaleRate;

	boss.SetScaleMultiplier(Math::Vector3(rate, rate, rate));
}
