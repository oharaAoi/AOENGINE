#include "BossIdleState.h"
#include "Game/Actor/Boss/Boss.h"

void BossIdleState::OnStart() {
	SetName("DeployArmor State");
	floatingTween_.Init(&floatingValue_, -0.5f, 0.5f, 1.5f, (int)EasingType::InOut::Sine, LoopType::RETURN);
}

void BossIdleState::OnUpdate() {
	floatingTween_.Update(GameTimer::DeltaTime());
	pOwner_->GetTransform()->temporaryTranslate_.y += floatingValue_;
}

void BossIdleState::OnExit() {
}
