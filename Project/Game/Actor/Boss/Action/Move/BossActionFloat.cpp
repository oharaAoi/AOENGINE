#include "BossActionFloat.h"
#include "Game/Actor/Boss/Boss.h"

BehaviorStatus BossActionFloat::Execute() {
	return Action();
}

float BossActionFloat::EvaluateWeight() {
	return 0.7f;
}

void BossActionFloat::Debug_Gui() {
	ITaskNode::Debug_Gui();
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
	pTarget_->GetTransform()->srt_.translate.y += 2.0f * GameTimer::DeltaTime();
}

void BossActionFloat::End() {
}
