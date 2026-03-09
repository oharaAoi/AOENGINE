#include "BossActionApproachFlamethrower.h"
#include "Game/Actor/Boss/Boss.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行処理
///////////////////////////////////////////////////////////////////////////////////////////////

BossActionApproachFlamethrower::BossActionApproachFlamethrower() {
	param_.Load();
}

BehaviorStatus BossActionApproachFlamethrower::Execute() {
	return Action();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 重みの計算
///////////////////////////////////////////////////////////////////////////////////////////////

float BossActionApproachFlamethrower::EvaluateWeight() {
	return 0.5f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionApproachFlamethrower::Debug_Gui() {
	BaseTaskNode::Debug_Gui();
}

void BossActionApproachFlamethrower::Parameter::Debug_Gui() {
	ImGui::DragFloat("speed", &moveSpeed, 0.1f);
	ImGui::DragFloat("moveTime", &moveTime, 0.1f);
	ImGui::DragFloat("lookTime", &lookTime, 0.1f);
	ImGui::DragFloat("minDecel", &minDecel, 0.1f);
	ImGui::DragFloat("maxDecel", &maxDecel, 0.1f);
	ImGui::DragFloat("stopThreshold", &stopThreshold, 0.1f);
	ImGui::DragFloat("backLength", &backLength, 0.1f);
	decelCurve.Debug_Gui();
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionApproachFlamethrower::IsFinish() {
	if (velocity_.Length() < param_.stopThreshold) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionApproachFlamethrower::CanExecute() {
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionApproachFlamethrower::Init() {
	pTarget_->SetIsMove(true);
	pTarget_->SetIsAttack(false);

	Math::Quaternion rot = pTarget_->GetTargetTransform()->GetRotate();
	Math::Vector3 front = rot.MakeForward() * -1.0f;
	Math::Vector3 tagetPos = (rot.MakeForward() * param_.backLength) + pTarget_->GetTargetPos();

	direction_ = (tagetPos - pTarget_->GetPosition()).Normalize();
	velocity_ = direction_ * param_.moveSpeed;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionApproachFlamethrower::Update() {
	taskTimer_ += AOENGINE::GameTimer::DeltaTime();
	if (taskTimer_ < param_.lookTime) {
		direction_ = (pTarget_->GetTargetPos() - pTarget_->GetPosition()).Normalize();
		pTarget_->TargetLook();
	}
	Approach();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionApproachFlamethrower::End() {
	pTarget_->SetIsMove(false);
	pTarget_->TargetLook();

	AOENGINE::Rigidbody* rigid = pTarget_->GetGameObject()->GetRigidbody();
	rigid->SetVelocity(CVector3::ZERO);
}

void BossActionApproachFlamethrower::Approach() {
	float dt = AOENGINE::GameTimer::DeltaTime();
	float t = taskTimer_ / param_.moveTime;
	t = std::clamp(t, 0.0f, 1.0f);

	float bezierValue = param_.decelCurve.BezierValue(t);
	float decelRate = std::lerp(param_.minDecel, param_.maxDecel, bezierValue);

	// 後半ほど入力を弱める
	float accelFactor = 1.0f - t;
	velocity_ += direction_ * param_.moveSpeed * accelFactor * dt;

	// 指数減衰
	velocity_ *= std::exp(-decelRate * dt);

	AOENGINE::Rigidbody* rigid = pTarget_->GetGameObject()->GetRigidbody();
	rigid->SetVelocity(velocity_);
}