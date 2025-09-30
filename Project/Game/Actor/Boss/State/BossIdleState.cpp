#include "BossIdleState.h"
#include "Game/Actor/Boss/Boss.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

void BossIdleState::OnStart() {
	SetName("IdleState");
	floatingTween_.Init(-0.5f, 0.5f, 1.5f, (int)EasingType::InOut::Sine, LoopType::RETURN);
}

void BossIdleState::OnUpdate() {
	floatingTween_.Update(GameTimer::DeltaTime());
	pOwner_->GetTransform()->temporaryTranslate_.y += floatingTween_.GetValue();
}

void BossIdleState::OnExit() {
}
