#include "BossActionFloat.h"
#include "Game/Actor/Boss/Boss.h"

BehaviorStatus BossActionFloat::Execute() {
	return Action();
}

void BossActionFloat::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());
}

bool BossActionFloat::IsFinish() {
	if (taskTimer_ > 1.0f) {
		return true;
	}

	return false;
}

bool BossActionFloat::CanExecute() {
	return true;
}

void BossActionFloat::Init() {
}

void BossActionFloat::Update() {
	taskTimer_ += GameTimer::DeltaTime();
	pTarget_->GetTransform()->translate_.y += 2.0f * GameTimer::DeltaTime();
}

void BossActionFloat::End() {
}
