#include "BossStateBeDestroyed.h"
#include "Game/Actor/Boss/Boss.h"
#include "Engine/Lib/Json/JsonItems.h"

void BossStateBeDestroyed::OnStart() {
	SetName("StanBeDestroyed");
	param_.FromJson(JsonItems::GetData(stateMachine_->GetName(), param_.GetName()));

	timer_ = 0.0f;
	GameTimer::SetTimeScale(0.2f);

	preRotate_ = pOwner_->GetTransform()->rotation_;
	
	Quaternion pitchRotate = Quaternion::AngleAxis(-45.0f * kToRadian, CVector3::RIGHT);
	targetRotate_ = preRotate_ * pitchRotate;
}

void BossStateBeDestroyed::OnUpdate() {
	timer_ += GameTimer::DeltaTime();

	float t = timer_ / param_.slowTime;
	t = std::clamp(t, 0.0f, param_.slowTime);
	pOwner_->GetTransform()->rotation_ = Quaternion::Slerp(preRotate_, targetRotate_, t);

	if (timer_ > param_.slowTime) {
		GameTimer::SetTimeScale(1.0f);
	}

	if (timer_ > param_.breakTime) {
		pOwner_->SetIsBreak(true);
	}
}

void BossStateBeDestroyed::OnExit() {
}

void BossStateBeDestroyed::Debug_Gui() {
	ImGui::DragFloat("slowTime", &param_.slowTime, 0.1f);
	ImGui::DragFloat("breakTime", &param_.breakTime, 0.1f);
	if (ImGui::Button("Save")) {
		JsonItems::Save(stateMachine_->GetName(), param_.ToJson(param_.GetName()));
	}
	if (ImGui::Button("Apply")) {
		param_.FromJson(JsonItems::GetData(stateMachine_->GetName(), param_.GetName()));
	}
}
