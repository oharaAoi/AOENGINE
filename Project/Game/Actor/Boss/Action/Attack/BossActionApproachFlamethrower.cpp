#include "BossActionApproachFlamethrower.h"
#include "Game/Actor/Boss/Boss.h"
#include "Engine/Lib/Math/Easing.h"

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
	param_.Debug_Gui();
	flamethrowerParam_.Debug_Gui();
}

void BossActionApproachFlamethrower::Parameter::Debug_Gui() {
	ImGui::DragFloat("closeLength", &closeLength, 0.1f);
	ImGui::DragFloat("stopLength", &stopLength, 0.1f);
	ImGui::DragFloat("dampingValue", &dampingValue, 0.1f);
	ImGui::DragFloat("speed", &speed, 0.1f);
	SaveAndLoad();
}

void BossActionApproachFlamethrower::AttackFlamethrower::Debug_Gui() {
	ImGui::DragFloat("closeTime", &closeTime);
	ImGui::DragFloat("startAngle", &startAngle);
	ImGui::DragFloat("endAngle", &endAngle);
	Math::SelectEasing(easeType, "##BossActionApproachFlamethrower");
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionApproachFlamethrower::IsFinish() {
	if (!closeTimer_.Run(0.0f)) {
		if (0.1f > velocity_.Length()) {
			return true;
		}
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

	pTarget_->GetFlamethrowers()->SetAttackType(FlamethrowerAttackType::Bullet);

	speed_ = param_.speed;

	Math::Quaternion rot = pTarget_->GetTargetTransform()->GetRotate();
	Math::Vector3 front = rot.MakeForward() * -1.0f;
	Math::Vector3 tagetPos = (rot.MakeForward()) + pTarget_->GetTargetPos();

	direction_ = (tagetPos - pTarget_->GetPosition()).Normalize();
	velocity_ += direction_ * speed_;

	pFlamethrowers_ = pTarget_->GetFlamethrowers();

	isClose_ = false;
	isDamping_ = false;

	closeTimer_ = AOENGINE::Timer(flamethrowerParam_.closeTime);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionApproachFlamethrower::Update() {
	if (!pTarget_->GetFlamethrowers()->Deploy()) {
		return;
	}

	taskTimer_ += AOENGINE::GameTimer::DeltaTime();
	direction_ = (pTarget_->GetTargetPos() - pTarget_->GetPosition()).Normalize();
	pTarget_->TargetLook();
	
	Approach();

	if (isClose_) {
		CloseFlamethrowers();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionApproachFlamethrower::End() {
	pTarget_->SetIsMove(false);
	pTarget_->TargetLook();

	AOENGINE::Rigidbody* rigid = pTarget_->GetGameObject()->GetRigidbody();
	rigid->SetVelocity(CVector3::ZERO);

	pTarget_->GetFlamethrowers()->Remove();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 近づく処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionApproachFlamethrower::Approach() {
	
	if (isDamping_) {
		velocity_ *= std::exp(-param_.dampingValue * AOENGINE::GameTimer::DeltaTime());
	} else {
		velocity_ += direction_ * speed_ * AOENGINE::GameTimer::DeltaTime();
	}

	float length = (pTarget_->GetTargetPos() - pTarget_->GetPosition()).Length();
	if (length < param_.closeLength) {
		isClose_ = true;
	}

	if (length < param_.stopLength) {
		isDamping_ = true;
	}

	AOENGINE::Rigidbody* rigid = pTarget_->GetGameObject()->GetRigidbody();
	rigid->SetVelocity(velocity_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 火炎放射を閉じる処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionApproachFlamethrower::CloseFlamethrowers() {
	// 火炎放射のアニメーション処理
	if (closeTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		float currentAngle = std::lerp(flamethrowerParam_.startAngle, flamethrowerParam_.endAngle, 
									   Math::CallEasing(flamethrowerParam_.easeType, closeTimer_.t_));

		for (uint32_t i = 0; i < 2; ++i) {
			BossFlamethrowersType type = BossFlamethrowersType(i);
			BossFlamethrowers::Parameter param = pFlamethrowers_->GetParameter(type);
			param.angle = currentAngle;
			pFlamethrowers_->SetParameter(param, type);
		}
	} else {
		isDamping_ = true;
	}
}

