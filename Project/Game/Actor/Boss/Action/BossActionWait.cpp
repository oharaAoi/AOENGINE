#include "BossActionWait.h"
#include "Game/Actor/Boss/Boss.h"

BehaviorStatus BossActionWait::Execute() {
	return Action();
}

void BossActionWait::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());
}

bool BossActionWait::IsFinish() {
	if (taskTimer_ > 5.0f) {
		return true;
	}
	return false;
}

bool BossActionWait::CanExecute() {
	return true;
}

void BossActionWait::Init() {
}

void BossActionWait::Update() {
	taskTimer_ += GameTimer::DeltaTime();
}

void BossActionWait::End() {
}
