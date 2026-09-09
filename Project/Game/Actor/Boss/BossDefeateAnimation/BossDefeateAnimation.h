#pragma once

// stl
#include <memory>

// game
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/BossDefeateAnimation/BaseBossDefeateAnimationBehavior.h"



namespace BossDefeate {
enum Phase {
	Explosion,
	End,
};
}

/// <summary>
/// bossの撃破時に行うアニメーション
/// </summary>
class BossDefeateAnimation {
public: // constructor

	BossDefeateAnimation() = default;
	~BossDefeateAnimation() = default;

public: // public method

	void Init();

	void Update(Boss& boss);

	void ChangeNext();

public: // accessor method

	bool IsFinish() const { return isFinish_; }

private: // private variables

	BossDefeate::Phase animationPhase_;

	std::unique_ptr<BaseBossDefeateAnimationBehavior> behavior_;

	bool isFinish_;

};