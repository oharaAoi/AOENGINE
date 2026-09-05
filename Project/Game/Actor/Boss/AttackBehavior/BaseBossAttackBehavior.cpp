#include"BaseBossAttackBehavior.h"


bool BaseBossAttackBehavior::WaitStartDelay(float deltaTime, float delay) {
	if (startDelayTimer_ >= delay) {
		return false;
	}

	startDelayTimer_ += deltaTime;
	return startDelayTimer_ < delay;
}