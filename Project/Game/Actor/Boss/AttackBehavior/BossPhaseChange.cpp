#include "BossPhaseChange.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include "Engine/Lib/Math/Easing.h"
#include <Core/Engine.h>
#include <System/Audio/SoundManager.h>
#include "Game/Actor/Boss/Boss.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//  流れの割り当て
///////////////////////////////////////////////////////////////////////////////////////////////

BossPhaseChange::BossPhaseChange() {

	// 待ちの2つはカメラを動かさない。ずらしを0のままにしておく
	stepUpdaters_[ToIndex(Step::StartWait)] = [this](Boss& boss, float) { UpdateWait(boss); };
	stepUpdaters_[ToIndex(Step::EndWait)] = [this](Boss& boss, float) { UpdateWait(boss); };
	stepUpdaters_[ToIndex(Step::Done)] = [this](Boss& boss, float) { UpdateWait(boss); };

	stepUpdaters_[ToIndex(Step::ZoomIn)] = [this](Boss& boss, float ratio) { UpdateZoomIn(boss, ratio); };
	stepUpdaters_[ToIndex(Step::Hold)] = [this](Boss& boss, float) { UpdateHold(boss); };
	stepUpdaters_[ToIndex(Step::ZoomOut)] = [this](Boss& boss, float ratio) { UpdateZoomOut(boss, ratio); };
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  開始
///////////////////////////////////////////////////////////////////////////////////////////////

void BossPhaseChange::Enter(Boss& boss) {

	elapsedTime_ = 0.0f;
	isFinished_ = false;
	hasStartedAnimation_ = false;
	hasPlayedEffect_ = false;

	step_ = Step::StartWait;
	stepTimer_ = 0.0f;

	// 寄せる前のカメラの奥行きを基準にする
	hasCamera_ = boss.HasCamera();
	cameraBaseZ_ = boss.GetCameraWorldPosition().z;

	// 切り替えの最中に殴られて素通りされないようにする
	boss.SetInvincible(true);

	// シーン側がこれを見てプレイヤーを止める
	boss.SetPhaseChanging(true);

	Apply(boss, 0.0f);

	// 揺れもエフェクトも、アニメーションを見せてから一緒に出す
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void BossPhaseChange::Update(Boss& boss, float deltaTime) {

	const BossParameter& param = boss.GetParameter();

	// カメラ・アニメーション・エフェクトは、どれも演出の開始からの経過時間で見る
	elapsedTime_ += deltaTime;

	if (!hasStartedAnimation_ && elapsedTime_ >= param.phaseChangeAnimationDelay) {
		hasStartedAnimation_ = true;
	}

	if (!hasPlayedEffect_ && elapsedTime_ >= param.phaseChangeEffectDelay) {
		boss.ShakeCamera(param.phaseChangeShake);
		boss.PlayPhaseChangeEffect();
		//　咆哮の音を入れる
		Engine::GetSoundManager()->Play("BossPhaseChange");
		hasPlayedEffect_ = true;
	}

	// 膨らんで戻る動きは、演出全体とは別の長さで進める
	const float pulseDuration = param.phaseChangeTime;
	float pulseRatio = 1.0f;
	if (pulseDuration > 0.0f) {
		pulseRatio = std::clamp(elapsedTime_ / pulseDuration, 0.0f, 1.0f);
	}
	UpdateScalePulse(boss, pulseRatio);

	if (step_ == Step::Done) {
		return;
	}

	// 今の流れの進み具合を出す。長さが0でも壊れないようにしておく
	stepTimer_ += deltaTime;
	const float duration = CalcStepDuration(boss);
	float ratio = 1.0f;
	if (duration > 0.0f) {
		ratio = std::clamp(stepTimer_ / duration, 0.0f, 1.0f);
	}

	stepUpdaters_[ToIndex(step_)](boss, ratio);

	if (ratio >= 1.0f) {
		ChangeStep(static_cast<Step>(ToIndex(step_) + 1));

		// 最後まで進んだらここで行動を終わらせる
		if (step_ == Step::Done) {
			isFinished_ = true;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  終了
///////////////////////////////////////////////////////////////////////////////////////////////

void BossPhaseChange::Exit(Boss& boss) {

	// 途中で切り替わった場合も、必ず元の大きさと状態へ戻す
	boss.SetScaleMultiplier(CVector3::UNIT);
	boss.SetInvincible(false);
	boss.SetPhaseChanging(false);

	// カメラを寄せたままにすると戻らなくなるので、ここで必ず戻す
	Apply(boss, 0.0f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  流れの切り替え
///////////////////////////////////////////////////////////////////////////////////////////////

void BossPhaseChange::ChangeStep(Step next) {
	step_ = next;
	stepTimer_ = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  流れの長さ
///////////////////////////////////////////////////////////////////////////////////////////////

float BossPhaseChange::CalcStepDuration(const Boss& boss) const {

	const BossParameter& param = boss.GetParameter();

	if (step_ == Step::StartWait) { return param.phaseChangeStartWait; }
	if (step_ == Step::ZoomIn) { return param.phaseChangeZoomTime; }
	if (step_ == Step::Hold) {
		// 戻し始めるのは演出開始からの絶対時刻。ここまでに使った時間を引く
		const float used = param.phaseChangeStartWait + param.phaseChangeZoomTime;
		return (std::max)(param.phaseChangeReturnDelay - used, 0.0f);
	}
	if (step_ == Step::ZoomOut) { return param.phaseChangeReturnTime; }
	if (step_ == Step::EndWait) { return param.phaseChangeEndWait; }

	return 0.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  カメラのずらし
///////////////////////////////////////////////////////////////////////////////////////////////

void BossPhaseChange::Apply(Boss& boss, float ratio) const {

	const BossParameter& param = boss.GetParameter();

	boss.SetCameraExtraOffset(param.phaseChangeCameraOffset * ratio);
	boss.SetWorldZOffset(CalcWorldZOffset(boss, ratio));
}

float BossPhaseChange::CalcWorldZOffset(const Boss& boss, float ratio) const {

	const BossParameter& param = boss.GetParameter();

	// カメラが無いと距離が測れないので、寄せた分だけ動かして相対距離を保つだけにする
	if (!hasCamera_) {
		return param.phaseChangeCameraOffset.z * ratio;
	}

	// 寄せた後のカメラの奥行き
	const float cameraZ = cameraBaseZ_ + param.phaseChangeCameraOffset.z * ratio;

	// そこから調整値の距離だけ離れたところが、演出中に置きたい奥行き
	const float targetZ = cameraZ + param.phaseChangeBossZDistance;

	// 通常の奥行きからの差を、進み具合ぶんだけ入れる
	return (targetZ - param.worldZ) * ratio;
}

void BossPhaseChange::UpdateZoomIn(Boss& boss, float ratio) const {

	const float eased = Math::CallEasing(boss.GetParameter().phaseChangeZoomEaseKind, ratio);
	Apply(boss, eased);
}

void BossPhaseChange::UpdateZoomOut(Boss& boss, float ratio) const {

	const float eased = Math::CallEasing(boss.GetParameter().phaseChangeReturnEaseKind, ratio);
	Apply(boss, 1.0f - eased);
}

void BossPhaseChange::UpdateHold(Boss& boss) const {
	Apply(boss, 1.0f);
}

void BossPhaseChange::UpdateWait(Boss& boss) const {
	Apply(boss, 0.0f);
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
