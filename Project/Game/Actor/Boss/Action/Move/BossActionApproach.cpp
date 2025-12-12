#include "BossActionApproach.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Actor/Boss/Boss.h"

BehaviorStatus BossActionApproach::Execute() {
	return Action();
}

float BossActionApproach::EvaluateWeight() {
	return 0.4f;
}

void BossActionApproach::Debug_Gui() {
	BaseTaskNode::Debug_Gui();
	initParam_.Debug_Gui();
}

void BossActionApproach::Parameter::Debug_Gui() {
	ImGui::DragFloat("moveSpeed", &moveSpeed, .1f);
	ImGui::DragFloat("moveTime", &moveTime, .1f);
	ImGui::DragFloat("deceleration", &deceleration, .1f);
	ImGui::DragFloat("maxSpinDistance", &maxSpinDistance, .1f);
	ImGui::DragFloat("quitApproachLength", &quitApproachLength, .1f);
	ImGui::DragFloat("decayRate", &decayRate, 0.1f);
	ImGui::DragFloat("rotateT", &rotateT, 0.1f);
	SaveAndLoad();
}

void BossActionApproach::Init() {
	initParam_.Load();
	param_ = initParam_;

	// player方向を計算する
	toPlayer_ = pTarget_->GetTargetPos() - pTarget_->GetPosition();
	distance_ = toPlayer_.Length();
	direToPlayer_ = toPlayer_.Normalize();

	lateral_ = Math::Vector3::Cross(CVector3::UP, direToPlayer_).Normalize();

	spinAmount_ = Clamp01(distance_ / param_.maxSpinDistance);
	offsetDire_ = direToPlayer_ + lateral_ * spinAmount_;
	offsetDire_ = offsetDire_.Normalize();

	isShot_ = true;
	stopping_ = false;
	pTarget_->SetIsMove(true);
	pTarget_->SetIsAttack(false);
}

void BossActionApproach::Update() {
	taskTimer_ += AOENGINE::GameTimer::DeltaTime();
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

void BossActionApproach::Approach() {
	pTarget_->GetTransform()->MoveVelocity(toPlayer_.Normalize() * param_.moveSpeed * AOENGINE::GameTimer::DeltaTime(), param_.rotateT);
}

void BossActionApproach::SpinApproach() {
	// player方向を計算する
	toPlayer_ = pTarget_->GetTargetPos() - pTarget_->GetPosition();
	distance_ = toPlayer_.Length();
	direToPlayer_ = toPlayer_.Normalize();

	// Y軸を上とした場合、横方向ベクトルを計算
	lateral_ = Math::Vector3::Cross(CVector3::UP, direToPlayer_).Normalize();

	// 旋回の強さを距離に応じて調整（例：最大1.0）
	spinAmount_ = Clamp01(distance_ / param_.maxSpinDistance);
	// 横にずらす（スピン方向はランダムまたは固定）
	offsetDire_ = direToPlayer_ + lateral_ * spinAmount_;
	offsetDire_ = offsetDire_.Normalize();

	// 移動をさせる
	pTarget_->GetTransform()->MoveVelocity(offsetDire_ * param_.moveSpeed * AOENGINE::GameTimer::DeltaTime(), param_.rotateT);

	param_.moveSpeed -= param_.deceleration * AOENGINE::GameTimer::DeltaTime();
}
