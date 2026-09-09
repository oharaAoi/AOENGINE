#pragma once

// engine
#include "Engine/Utilities/Timer.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"

// game
#include "Game/Actor/Boss/BossDefeateAnimation/BaseBossDefeateAnimationBehavior.h"

class Boss;
class BossDefeateAnimation;

class BossDefeateExlosionAnimation :
	public BaseBossDefeateAnimationBehavior {
public:

	BossDefeateExlosionAnimation(BossDefeateAnimation* bossDefeateAnimation) :
		BaseBossDefeateAnimationBehavior(bossDefeateAnimation) {
	}
	~BossDefeateExlosionAnimation() override = default;

public:

	void Init() override;

	void Update(Boss& boss) override;

private:

	BossExplosionParameter explosionParameter_;

	AOENGINE::BaseParticles* flashParticle_;

	AOENGINE::Timer timer_;
	AOENGINE::Timer finalExplosionTimer_;

	int count_ = 0;
};

