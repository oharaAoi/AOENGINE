#include "BossActionWait.h"
#include "Game/Actor/Boss/Boss.h"

BehaviorStatus BossActionWait::Execute() {
	if (state_ == BehaviorStatus::Inactive) {
		Init();
	}

	Update();

	if (actionTimer_ > 5.0f) {
		End();
		return BehaviorStatus::Success;
	}
	return BehaviorStatus::Running;
}

void BossActionWait::Init() {
	SetNodeName("actionIdle");
	state_ = BehaviorStatus::Running;
	actionTimer_ = 0;
}

void BossActionWait::Update() {
	actionTimer_++;
}

void BossActionWait::End() {
	state_ = BehaviorStatus::Inactive;
}
