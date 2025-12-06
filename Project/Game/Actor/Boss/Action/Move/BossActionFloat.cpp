#include "BossActionFloat.h"
#include "Game/Actor/Boss/Boss.h"
#include "Engine/Lib/Json/JsonItems.h"


BehaviorStatus BossActionFloat::Execute() {
	return Action();
}

float BossActionFloat::EvaluateWeight() {
	return 0.7f;
}

void BossActionFloat::Debug_Gui() {
	BaseTaskNode::Debug_Gui();
	param_.Debug_Gui();
}

void BossActionFloat::Parameter::Debug_Gui() {
	ImGui::DragFloat("moveTime", &moveTime, 0.1f);
	ImGui::DragFloat("moveSpeed", &moveSpeed, 0.1f);
	SaveAndLoad();
}

bool BossActionFloat::IsFinish() {
	if (taskTimer_ > param_.moveTime) {
		return true;
	}

	return false;
}

bool BossActionFloat::CanExecute() {
	return true;
}

void BossActionFloat::Init() {
	param_.SetGroupName("BossAction");
	param_.Load();
	pTarget_->SetIsMove(true);
	pTarget_->SetIsAttack(false);
}

void BossActionFloat::Update() {
	taskTimer_ += AOENGINE::GameTimer::DeltaTime();
	pTarget_->GetTransform()->srt_.translate.y += param_.moveTime * AOENGINE::GameTimer::DeltaTime();
}

void BossActionFloat::End() {
	pTarget_->SetIsMove(false);
}
