#include "PlayerIdleState.h"
#include "Game/Actor/Player/Player.h"

void PlayerIdleState::OnStart() {
	floatingTween_.Init(&floatingValue_, -0.2f, 0.2f, 1.0f, (int)EasingType::InOut::Sine, LoopType::RETURN);
}

void PlayerIdleState::OnUpdate() {
	floatingTween_.Update(GameTimer::DeltaTime());
	pOwner_->GetTransform()->temporaryTranslate_.y += floatingValue_;
}

void PlayerIdleState::OnExit() {
}
