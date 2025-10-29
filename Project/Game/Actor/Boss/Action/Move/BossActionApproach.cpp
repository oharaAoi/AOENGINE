#include "BossActionApproach.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Actor/Boss/Boss.h"

BehaviorStatus BossActionApproach::Execute() {
	return Action();
}

float BossActionApproach::EvaluateWeight() {
	return pTarget_->GetEvaluationFormula()->ApproachEvaluation(50.0f, 100.0f);
}

void BossActionApproach::Debug_Gui() {
	ITaskNode::Debug_Gui();
	initParam_.Debug_Gui();
}

void BossActionApproach::Parameter::Debug_Gui() {
	ImGui::DragFloat("moveSpeed", &moveSpeed, .1f);
	ImGui::DragFloat("moveTime", &moveTime, .1f);
	ImGui::DragFloat("deceleration", &deceleration, .1f);
	ImGui::DragFloat("maxSpinDistance", &maxSpinDistance, .1f);
	ImGui::DragFloat("quitApproachLength", &quitApproachLength, .1f);
	ImGui::DragFloat("decayRate", &decayRate, 0.1f);
	ImGui::DragFloat("recoveryTime", &recoveryTime, 0.1f);
	SaveAndLoad();
}

void BossActionApproach::Init() {
	initParam_.SetGroupName("BossAction");
	initParam_.Load();
	param_ = initParam_;

	// player方向を計算する
	toPlayer_ = pTarget_->GetPlayerPosition() - pTarget_->GetPosition();
	distance_ = toPlayer_.Length();
	direToPlayer_ = toPlayer_.Normalize();

	lateral_ = Vector3::Cross(CVector3::UP, direToPlayer_).Normalize();

	spinAmount_ = Clamp01(distance_ / param_.maxSpinDistance);
	offsetDire_ = direToPlayer_ + lateral_ * spinAmount_;
	offsetDire_ = offsetDire_.Normalize();

	isShot_ = true;
	stopping_ = false;
	pTarget_->SetIsMove(true);
	pTarget_->SetIsAttack(false);

	waitTimer_.targetTime_ = param_.recoveryTime;
}

void BossActionApproach::Update() {
	taskTimer_ += GameTimer::DeltaTime();
	Approach();

	if (taskTimer_ > param_.moveTime) {
		stopping_ = true;
	}
}

void BossActionApproach::End() {
	pTarget_->SetIsMove(false);
}

bool BossActionApproach::IsFinish() {
	if (distance_ < param_.quitApproachLength) {
		return true;
	}

	if (taskTimer_ > initParam_.moveTime) {
		return true;
	}
	return false;
}

bool BossActionApproach::CanExecute() {
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次のアクションのチェック
///////////////////////////////////////////////////////////////////////////////////////////////

//void BossActionApproach::CheckNextAction() {
//	if (distance_ < param_.quitApproachLength) {
//		size_t hash = pTarget_->GetAI()->AttackActionAI();
//		NextAction(hash);
//	}
//
//	if (actionTimer_ > initParam_.moveTime) {
//		size_t hash = pTarget_->GetAI()->AttackActionAI();
//		NextAction(hash);
//	}
//}

void BossActionApproach::Approach() {
	pTarget_->GetTransform()->MoveVelocity(toPlayer_.Normalize() * param_.moveSpeed * GameTimer::DeltaTime(), 0.1f);
}

void BossActionApproach::SpinApproach() {
	// player方向を計算する
	toPlayer_ = pTarget_->GetPlayerPosition() - pTarget_->GetPosition();
	distance_ = toPlayer_.Length();
	direToPlayer_ = toPlayer_.Normalize();

	// Y軸を上とした場合、横方向ベクトルを計算
	lateral_ = Vector3::Cross(CVector3::UP, direToPlayer_).Normalize();

	// 旋回の強さを距離に応じて調整（例：最大1.0）
	spinAmount_ = Clamp01(distance_ / param_.maxSpinDistance);
	// 横にずらす（スピン方向はランダムまたは固定）
	offsetDire_ = direToPlayer_ + lateral_ * spinAmount_;
	offsetDire_ = offsetDire_.Normalize();

	// 移動をさせる
	pTarget_->GetTransform()->MoveVelocity(offsetDire_ * param_.moveSpeed * GameTimer::DeltaTime(), 0.1f);

	param_.moveSpeed -= param_.deceleration * GameTimer::DeltaTime();
}
