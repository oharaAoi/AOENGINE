#include "BossAnimation.h"

#include "Engine/Module/Components/Animation/AnimationClip.h"
#include "Engine/Module/Components/Animation/Animator.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//  初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAnimation::Init() {
	isDamagePlaying_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAnimation::Update(const Context& context, const Params& params) {

	if (context.animator == nullptr) {
		return;
	}

	UpdateDamageState(context);

	const std::string& next = SelectName(context);

	// 補間の途中で別の遷移を投げると、AnimationClip側は
	// 「完了済みのクリップ名 == 投げた名前」で弾いてしまい、
	// こちらの認識と実際に流れているクリップがずれて固まる。
	// 補間中は何も投げず、終わってから実際の名前と見比べて切り替える
	if (context.animator->GetIsAnimationChange()) {
		return;
	}

	if (context.animator->GetAnimationName() == next) {
		return;
	}

	context.animator->TransitionAnimation(next, params.blendSpeed);
	context.animator->SetAnimationSpeed(params.speed);

	// 被弾は1回だけ。それ以外はずっと繰り返す
	if (AOENGINE::AnimationClip* clip = context.animator->GetAnimationClip()) {
		clip->SetIsLoop(next != kDamageName);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  被弾
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAnimation::PlayDamage() {
	isDamagePlaying_ = true;
}

void BossAnimation::UpdateDamageState(const Context& context) {

	if (!isDamagePlaying_) {
		return;
	}

	// 被弾が実際に流れ始めてから、その再生が終わったかを見る。
	// 名前を確かめないと、行動側のループが一周した時の終了フラグを拾ってしまう
	if (context.animator->GetAnimationName() != kDamageName) {
		return;
	}

	const AOENGINE::AnimationClip* clip = context.animator->GetAnimationClip();
	if (clip == nullptr) {
		return;
	}

	if (clip->GetIsAnimationFinish()) {
		isDamagePlaying_ = false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  流すアニメーションを決める
///////////////////////////////////////////////////////////////////////////////////////////////

const std::string& BossAnimation::SelectName(const Context& context) const {

	// 被弾は行動に割り込む
	if (isDamagePlaying_) {
		return kDamageName;
	}

	if (context.behaviorName == nullptr || context.behaviorName->empty()) {
		return kDefaultName;
	}

	return *context.behaviorName;
}
