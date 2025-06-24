#include "BossStateNormal.h"
#include "Game/Actor/Boss/Boss.h"

void BossStateNormal::OnStart() {
	SetName("NormalState");
	floatingTween_.Init(&floatingValue_, -0.5f, 0.5f, 1.5f, (int)EasingType::InOut::Sine, LoopType::RETURN);
}

void BossStateNormal::OnUpdate() {
	floatingTween_.Update(GameTimer::DeltaTime());
	pOwner_->GetTransform()->temporaryTranslate_.y += floatingValue_;
}

void BossStateNormal::OnExit() {
}

void BossStateNormal::Debug_Gui() {
}
