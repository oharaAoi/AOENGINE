#include "PlayerIdleState.h"
#include "Game/Actor/Player/Player.h"

void PlayerIdleState::OnStart() {
	floatingTween_.Init(-0.2f, 0.2f, 1.0f, (int)EasingType::InOut::Sine, LoopType::Return);
}

void PlayerIdleState::OnUpdate() {
	floatingTween_.Update(GameTimer::DeltaTime());
}

void PlayerIdleState::OnExit() {
}
