#include "PlayerKnockbackState.h"
#include "Engine/Lib/GameTimer.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/State/PlayerIdleState.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"

void PlayerKnockbackState::OnStart() {
	timer_ = 0.0f;
	knockbackTime_ = 0.6f;

	strength_ = 150.0f;

	velocity_;
	acceleration_;

	glitchNoise_ = Engine::GetPostProcess()->GetGlitchNoise();
	glitchNoise_->StartNoise(1.0f, knockbackTime_ * 0.6f);
}

void PlayerKnockbackState::OnUpdate() {
	Knockback();
}

void PlayerKnockbackState::OnExit() {
	pOwner_->SetKnockback(false);

	size_t hash = typeid(PlayerActionIdle).hash_code();
	pOwner_->GetActionManager()->ChangeAction(hash);
}

void PlayerKnockbackState::Knockback() {
	timer_ += GameTimer::DeltaTime();
	strength_ *= 0.8f;

	Vector3 direction = pOwner_->GetKnockBackDire();
	acceleration_ = (direction * strength_) * GameTimer::DeltaTime();
	velocity_ += acceleration_ * GameTimer::DeltaTime();
	pOwner_->GetTransform()->translate_ += velocity_;

	if (timer_ > knockbackTime_) {
		stateMachine_->ChangeState<PlayerIdleState>();
	}
}
