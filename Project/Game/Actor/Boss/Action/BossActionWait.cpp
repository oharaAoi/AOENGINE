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

void BossActionWait::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());
}

void BossActionWait::Init() {
	state_ = BehaviorStatus::Running;
	actionTimer_ = 0;
}

void BossActionWait::Update() {
	actionTimer_ += GameTimer::DeltaTime();
}

void BossActionWait::End() {
	state_ = BehaviorStatus::Inactive;
}
