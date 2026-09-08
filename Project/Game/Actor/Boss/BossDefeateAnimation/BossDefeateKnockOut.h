#pragma once

// engine
#include "Engine/Utilities/Timer.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"

// game
#include "Game/Actor/Boss/BossDefeateAnimation/BaseBossDefeateAnimationBehavior.h"

class Boss;
class BossDefeateAnimation;

class BossDefeateKnockOut :
	public BaseBossDefeateAnimationBehavior {
public:

	BossDefeateKnockOut(BossDefeateAnimation* bossDefeateAnimation) :
		BaseBossDefeateAnimationBehavior(bossDefeateAnimation) {
	}
	~BossDefeateKnockOut() override = default;

public:

	void Init() override;

	void Update(Boss& boss) override;

private:

	BossKnockOutParameter knockOutParameter_;

};

