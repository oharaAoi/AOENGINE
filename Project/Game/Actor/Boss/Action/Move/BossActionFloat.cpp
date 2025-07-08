#include "BossActionFloat.h"
#include "Game/Actor/Boss/Boss.h"

BehaviorStatus BossActionFloat::Execute() {
	if (state_ == BehaviorStatus::Inactive) {
		Init();
	}

	Update();

	if (actionTimer_ > 2.0f) {
		End();
		return BehaviorStatus::Success;
	}
	return BehaviorStatus::Running;
}

void BossActionFloat::Init() {
	state_ = BehaviorStatus::Running;
	actionTimer_ = 0;
}

void BossActionFloat::Update() {
	actionTimer_ += GameTimer::DeltaTime();
	pTarget_->GetTransform()->translate_.y += 4.0f * GameTimer::DeltaTime();
}

void BossActionFloat::End() {
	state_ = BehaviorStatus::Inactive;
}
