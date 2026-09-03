#include "PlayerJump.h"

PlayerJump::PlayerJump() {
	// --- 接地 ---
	stateUpdaters_[ToIndex(State::Grounded)] = [this](float deltaTime, bool jumpTriggered) {
		(void)deltaTime;
		// 足場へ軽く押し付けて接地を維持する
		velocityY_ = -params_.groundKeepSpeed;
		if (jumpTriggered) {
			velocityY_ = params_.jumpPower;
			ChangeState(State::Rising);
			jumpStarted_ = true;
		}
	};

	// --- 上昇中 ---
	stateUpdaters_[ToIndex(State::Rising)] = [this](float deltaTime, bool) {
		velocityY_ -= params_.riseGravity * deltaTime;
		if (velocityY_ <= 0.0f) {
			// 頂点に到達 → 滞空へ
			velocityY_ = 0.0f;
			ChangeState(State::Hanging);
		}
	};

	// --- 頂点で滞空中 ---
	stateUpdaters_[ToIndex(State::Hanging)] = [this](float deltaTime, bool) {
		// 滞空中は高さを維持する
		velocityY_ = 0.0f;
		hangTimer_ += deltaTime;
		if (hangTimer_ >= params_.hangTime) {
			ChangeState(State::Falling);
		}
	};

	// --- 落下中 ---
	stateUpdaters_[ToIndex(State::Falling)] = [this](float deltaTime, bool) {
		velocityY_ -= params_.fallGravity * deltaTime;
		if (velocityY_ < -params_.maxFallSpeed) {
			velocityY_ = -params_.maxFallSpeed;
		}
	};
}

void PlayerJump::Update(float deltaTime, bool jumpTriggered, const Params& params) {
	jumpStarted_ = false;
	params_ = params;
	stateUpdaters_[ToIndex(state_)](deltaTime, jumpTriggered);
}

void PlayerJump::Land() {
	if (velocityY_ < 0.0f) {
		velocityY_ = 0.0f;
	}
	// 着地へ切り替え
	if (state_ == State::Falling || state_ == State::Hanging) {
		ChangeState(State::Grounded);
	}
}

void PlayerJump::LeaveGround() {
	// 足場から外れたら落下を始める
	if (state_ != State::Grounded) {
		return;
	}
	velocityY_ = 0.0f;
	ChangeState(State::Falling);
}

void PlayerJump::HitCeiling() {
	// 頭をぶつけたら上昇を打ち切る
	if (state_ != State::Rising && state_ != State::Hanging) {
		return;
	}
	velocityY_ = 0.0f;
	ChangeState(State::Falling);
}

void PlayerJump::Knockback(float power) {
	// 普通のジャンプと同じ流れを、強い初速で割り込んで開始するだけ
	velocityY_ = power;
	ChangeState(State::Rising);
}

void PlayerJump::ChangeState(State next) {
	// State切り替え
	state_ = next;
	if (next == State::Hanging) {
		hangTimer_ = 0.0f;
	}
}

const std::string& PlayerJump::GetStateName() const {

	// 状態名取得
	static const std::array<std::string, kStateCount> kNames = {
		"Grounded", "Rising", "Hanging", "Falling",
	};
	return kNames[ToIndex(state_)];
}
