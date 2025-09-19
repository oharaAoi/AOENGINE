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
	ITaskNode::Debug_Gui();
	ImGui::DragFloat("moveTime", &param_.moveTime, 0.1f);
	ImGui::DragFloat("moveSpeed", &param_.moveSpeed, 0.1f);
	if (ImGui::Button("Save")) {
		JsonItems::Save("BossAction", param_.ToJson(param_.GetName()));
	}
	if (ImGui::Button("Apply")) {
		param_.FromJson(JsonItems::GetData("BossAction", param_.GetName()));
	}
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
	param_.FromJson(JsonItems::GetData("BossAction", param_.GetName()));
	pTarget_->SetIsMove(true);
	pTarget_->SetIsAttack(false);
}

void BossActionFloat::Update() {
	taskTimer_ += GameTimer::DeltaTime();
	pTarget_->GetTransform()->srt_.translate.y += param_.moveTime * GameTimer::DeltaTime();
}

void BossActionFloat::End() {
	pTarget_->SetIsMove(false);
}
