#include "BossActionFloat.h"
#include "Game/Actor/Boss/Boss.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"


BossActionFloat::BossActionFloat() {
	param_.Load();
	evaluator_.maxValue = param_.maxDistance;
}

BehaviorStatus BossActionFloat::Execute() {
	return Action();
}

float BossActionFloat::EvaluateWeight() {
	float distance = std::abs(pTarget_->GetTargetPos().y - pTarget_->GetPosition().y);
	return evaluator_.Evaluate(distance);
}

void BossActionFloat::Debug_Gui() {
	BaseTaskNode::Debug_Gui();
	param_.Debug_Gui();
}

void BossActionFloat::Parameter::Debug_Gui() {
	ImGui::DragFloat("moveTime", &moveTime, 0.1f);
	ImGui::DragFloat("moveSpeed", &moveSpeed, 0.1f);
	ImGui::DragFloat("appropriateDistance", &appropriateDistance, 0.1f);
	ImGui::DragFloat("maxDistance", &maxDistance, 0.1f);
	moveCurve.Debug_Gui();
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
	pTarget_->SetIsMove(true);
	pTarget_->SetIsAttack(false);
}

void BossActionFloat::Update() {
	taskTimer_ += AOENGINE::GameTimer::DeltaTime();
	float t = taskTimer_ / param_.moveTime;
	float bezierValue = param_.moveCurve.BezierValue(t);

	AOENGINE::Rigidbody* rigid = pTarget_->GetGameObject()->GetRigidbody();
	rigid->SetMoveForce(Math::Vector3(0, (param_.moveSpeed * bezierValue) * AOENGINE::GameTimer::DeltaTime(), 0));
}

void BossActionFloat::End() {
	pTarget_->SetIsMove(false);
}
