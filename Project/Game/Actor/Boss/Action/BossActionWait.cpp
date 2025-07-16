#include "BossActionWait.h"
#include "Game/Actor/Boss/Boss.h"

BehaviorStatus BossActionWait::Execute() {
	return Action();
}

float BossActionWait::EvaluateWeight() {
	return 0.0f;
}

void BossActionWait::Debug_Gui() {
	ITaskNode::Debug_Gui();
}

bool BossActionWait::IsFinish() {
	if (taskTimer_ > 2.0f) {
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
