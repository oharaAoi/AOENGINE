#include "PlayerKnockbackState.h"
#include "Engine/Lib/GameTimer.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/State/PlayerIdleState.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 開始処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerKnockbackState::OnStart() {
	timer_ = 0.0f;
	knockbackTime_ = 2.0f;

	strength_ = 150.0f;

	velocity_;
	acceleration_;

	glitchNoise_ = Engine::GetPostProcess()->GetGlitchNoise();
	glitchNoise_->StartNoise(1.0f, knockbackTime_ * 0.6f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerKnockbackState::OnUpdate() {
	Knockback();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerKnockbackState::OnExit() {
	pOwner_->SetKnockback(false);

	size_t hash = typeid(PlayerActionIdle).hash_code();
	pOwner_->GetActionManager()->ChangeAction(hash);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ ノックバック処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerKnockbackState::Knockback() {
	timer_ += GameTimer::DeltaTime();
	strength_ *= 0.8f;

	// ノックバック方向のベクトルを取得
	Vector3 direction = pOwner_->GetKnockBackDire();
	acceleration_ = (direction * strength_) * GameTimer::DeltaTime();
	velocity_ += acceleration_ * GameTimer::DeltaTime();
	pOwner_->GetTransform()->srt_.translate += velocity_;

	if (timer_ > knockbackTime_) {
		stateMachine_->ChangeState<PlayerIdleState>();
		pOwner_->SetPostureStability(0.0f);
	}
}
