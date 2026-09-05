#include "BossAnimation.h"

#include "Engine/Module/Components/Animation/AnimationClip.h"
#include "Engine/Module/Components/Animation/Animator.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//  初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAnimation::Init() {
	isDamagePlaying_ = false;
	isReplayRequested_ = false;
	isBehaviorFinished_ = false;
	lastBehaviorName_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAnimation::Update(const Context& context, const Params& params) {

	if (context.animator == nullptr) {
		return;
	}

	// 行動が変わったら、流し切ったかどうかを数え直す
	const std::string& behaviorName = SelectBehaviorName(context);
	if (behaviorName != lastBehaviorName_) {
		lastBehaviorName_ = behaviorName;
		isBehaviorFinished_ = false;
	}

	UpdateDamageState(context);
	UpdateBehaviorFinishState(context, behaviorName);

	const std::string& next = SelectName(context);

	// 同じクリップを頭から流し直す。補間中はそのまま待って次のフレームで行う
	if (isReplayRequested_ && !context.animator->GetIsAnimationChange()) {
		isReplayRequested_ = false;
		if (AOENGINE::AnimationClip* clip = context.animator->GetAnimationClip()) {
			clip->ResetAnimation(next);
			clip->SetIsLoop(next == kIdleName);
		}
		return;
	}

。
	// 補間中は何も投げず、終わってから実際の名前と見比べて切り替える
	if (context.animator->GetIsAnimationChange()) {
		return;
	}

	if (context.animator->GetAnimationName() == next) {
		return;
	}

	context.animator->TransitionAnimation(next, params.blendSpeed);
	context.animator->SetAnimationSpeed(params.speed);

	// 繰り返すのは待機だけ。攻撃と被弾は1回流したら止まる
	if (AOENGINE::AnimationClip* clip = context.animator->GetAnimationClip()) {
		clip->SetIsLoop(next == kIdleName);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  クリップが流れ切ったか
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossAnimation::IsClipFinished(const Context& context, const std::string& name) const {

	// 補間中は前のクリップの名前が返るので、切り替わり切ってから見る
	if (context.animator->GetIsAnimationChange()) {
		return false;
	}

	if (context.animator->GetAnimationName() != name) {
		return false;
	}

	const AOENGINE::AnimationClip* clip = context.animator->GetAnimationClip();
	if (clip == nullptr) {
		return false;
	}


	const float duration = clip->GetAnimationDuration();
	if (duration <= 0.0f) {
		return false;
	}

	return clip->GetAnimationTime() >= duration;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  被弾
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAnimation::PlayDamage() {
	isDamagePlaying_ = true;
}

void BossAnimation::Replay() {
	isReplayRequested_ = true;
	// 流し直すので、終わった扱いも取り消す
	isBehaviorFinished_ = false;
}

void BossAnimation::UpdateDamageState(const Context& context) {

	if (!isDamagePlaying_) {
		return;
	}

	// 被弾が実際に流れ始めてから、その再生が終わったかを見る
	if (IsClipFinished(context, kDamageName)) {
		isDamagePlaying_ = false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  行動のアニメーションが流れ切ったか
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAnimation::UpdateBehaviorFinishState(const Context& context, const std::string& behaviorName) {

	// 既に終わっている、被弾中、そもそも待機なら見る必要が無い
	if (isBehaviorFinished_ || isDamagePlaying_ || behaviorName == kIdleName) {
		return;
	}

	// 今まさにその行動のアニメーションが流れ切ったか
	if (IsClipFinished(context, behaviorName)) {
		isBehaviorFinished_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  流すアニメーションを決める
///////////////////////////////////////////////////////////////////////////////////////////////

const std::string& BossAnimation::SelectBehaviorName(const Context& context) const {

	if (context.behaviorName == nullptr || context.behaviorName->empty()) {
		return kIdleName;
	}

	return *context.behaviorName;
}

const std::string& BossAnimation::SelectName(const Context& context) const {

	// 被弾は行動に割り込む
	if (isDamagePlaying_) {
		return kDamageName;
	}

	// 攻撃アニメーションが流れ切ったら、最後の姿勢で固まらないように待機へ戻す
	if (isBehaviorFinished_) {
		return kIdleName;
	}

	return SelectBehaviorName(context);
}
