#include "PlayerAnimation.h"

#include <cmath>

#include "Engine/Module/Components/Animation/Animator.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//  初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerAnimation::Init() {
	noMoveInputTimer_ = 0.0f;
	currentName_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerAnimation::Update(float deltaTime, const Context& context, const Params& params) {

	// 左右の切り返しでは、片方を離してからもう片方を押すまでの数フレーム入力が0になる。
	// そこで即idleへ落とすと歩きが途切れて直立に見えるので、途切れた時間を数えておく
	if (IsMoving(context, params)) {
		noMoveInputTimer_ = 0.0f;
	} else {
		noMoveInputTimer_ += deltaTime;
	}

	if (context.animator == nullptr) {
		return;
	}

	const std::string& next = SelectName(context, params);
	currentName_ = next;

	// 補間中は何も投げず、終わってから実際の名前と見比べて切り替える
	if (context.animator->GetIsAnimationChange()) {
		return;
	}

	if (context.animator->GetAnimationName() == next) {
		return;
	}

	context.animator->TransitionAnimation(next, params.blendSpeed);
	context.animator->SetAnimationSpeed(SelectSpeed(next, params));
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  移動しているか
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerAnimation::IsMoving(const Context& context, const Params& params) const {

	// 移動している
	if (std::abs(context.horizontal) > params.moveInputThreshold) {
		return true;
	}

	// 速度で判定
	return std::abs(context.velocityX) > params.moveInputThreshold;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  流すアニメーションを決める
///////////////////////////////////////////////////////////////////////////////////////////////

const std::string& PlayerAnimation::SelectName(const Context& context, const Params& params) const {

	// 上昇・滞空・落下はまとめてジャンプ扱い
	if (!context.isGrounded) {
		return kJumpName;
	}

	if (noMoveInputTimer_ < params.idleDelay) {
		return kWalkName;
	}

	return kIdleName;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  再生速度を決める
///////////////////////////////////////////////////////////////////////////////////////////////

float PlayerAnimation::SelectSpeed(const std::string& name, const Params& params) const {

	// 歩きだけ少し速く回す
	if (name == kWalkName) {
		return params.walkSpeed;
	}

	return params.defaultSpeed;
}
