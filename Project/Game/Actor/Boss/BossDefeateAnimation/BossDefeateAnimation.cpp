#include "BossDefeateAnimation.h"
#include "Game/Actor/Boss/BossDefeateAnimation/BossDefeateExlosionAnimation.h"
#include "Game/Actor/Boss/BossDefeateAnimation/BossDefeateKnockOut.h"

class Boss;

void BossDefeateAnimation::Init() {
	behavior_ = std::make_unique<BossDefeateExlosionAnimation>(this);
	behavior_->Init();
	animationPhase_ = BossDefeate::Phase::Explosion;
	isFinish_ = false;
}

void BossDefeateAnimation::Update(Boss& boss) {
	behavior_->Update(boss);
}

void BossDefeateAnimation::ChangeNext() {
	switch (animationPhase_) {
	case BossDefeate::Phase::Explosion:
		behavior_ = std::make_unique<BossDefeateKnockOut>(this);
		behavior_->Init();
		break;
	default:
		break;
	}
}
