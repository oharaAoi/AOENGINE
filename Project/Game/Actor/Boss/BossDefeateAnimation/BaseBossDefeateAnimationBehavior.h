#pragma once

#include "Game/Actor/Boss/BossDefeateAnimation/BossDefeateAnimationParameters.h"

class Boss;
class BossDefeateAnimation;

class BaseBossDefeateAnimationBehavior {
public: 

	BaseBossDefeateAnimationBehavior(BossDefeateAnimation* bossDefeateAnimation) :
		animation_(bossDefeateAnimation) {
	}
	virtual ~BaseBossDefeateAnimationBehavior() = default;

public: 

	virtual void Init() = 0;

	virtual void Update(Boss& boss) = 0;

	void Next();

protected: 

	BossDefeateAnimation* animation_;

};

