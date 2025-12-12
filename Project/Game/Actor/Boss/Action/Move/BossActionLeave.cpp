#include "BossActionLeave.h"
#include "Game/Actor/Boss/Boss.h"

BehaviorStatus BossActionLeave::Execute() {
	return Action();
}

float BossActionLeave::EvaluateWeight() {
	return 0.4f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionLeave::Debug_Gui() {
	BaseTaskNode::Debug_Gui();
	param_.Debug_Gui();
}

void BossActionLeave::Parameter::Debug_Gui() {
	ImGui::DragFloat("moveSpeed", &moveSpeed, 0.1f);
	ImGui::DragFloat("moveTime", &moveTime, 0.1f);
	ImGui::DragFloat("decayRate", &decayRate, 0.1f);
	ImGui::DragFloat("rotateT", &rotateT, 0.1f);
	ImGui::DragFloat("finishDistance", &finishDistance, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionLeave::IsFinish() {
	if (stopping_) {
		if (velocity_.Length() <= param_.finishDistance) {
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionLeave::CanExecute() {
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionLeave::Init() {
	param_.Load();
	taskTimer_ = 0;

	stopping_ = false;

	accel_ = pTarget_->GetTransform()->srt_.rotate.MakeForward() * param_.moveSpeed;
	velocity_ = CVector3::ZERO;

	pTarget_->SetIsMove(true);
	pTarget_->SetIsAttack(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionLeave::Update() {
	taskTimer_ += AOENGINE::GameTimer::DeltaTime();

	if (!stopping_) {
		Leave();
	} else {
		Stop();
	}

	if (taskTimer_ > param_.moveTime) {
		stopping_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionLeave::End() {
	pTarget_->SetIsMove(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 離れる処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionLeave::Leave() {
	velocity_ += accel_ * AOENGINE::GameTimer::DeltaTime();
	pTarget_->GetTransform()->MoveVelocity(velocity_* AOENGINE::GameTimer::DeltaTime(), param_.rotateT);
}

void BossActionLeave::Stop() {
	velocity_ *= std::exp(-param_.decayRate * AOENGINE::GameTimer::DeltaTime());
	pTarget_->GetTransform()->srt_.translate += velocity_ * AOENGINE::GameTimer::DeltaTime();

	Math::Quaternion playerToRotate_ = Math::Quaternion::LookAt(pTarget_->GetPosition(), pTarget_->GetTargetPos());
	pTarget_->GetTransform()->srt_.rotate = Math::Quaternion::Slerp(pTarget_->GetTransform()->srt_.rotate, playerToRotate_, param_.rotateT);
}