#include "BossStateBeDestroyed.h"
#include "Game/Actor/Boss/Boss.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Manager/ParticleManager.h"

void BossStateBeDestroyed::OnStart() {
	SetName("BossStateBeDestroyed");

	param_.SetGroupName(stateMachine_->GetName());
	param_.Load();

	timer_ = 0.0f;
	GameTimer::SetTimeScale(0.2f);

	preRotate_ = pOwner_->GetTransform()->srt_.rotate;
	
	Quaternion pitchRotate = Quaternion::AngleAxis(-45.0f * kToRadian, CVector3::RIGHT);
	targetRotate_ = preRotate_ * pitchRotate;
}

void BossStateBeDestroyed::OnUpdate() {
	timer_ += GameTimer::DeltaTime();

	float t = timer_ / param_.slowTime;
	t = std::clamp(t, 0.0f, param_.slowTime);
	pOwner_->GetTransform()->srt_.rotate = Quaternion::Slerp(preRotate_, targetRotate_, t);

	if (timer_ > param_.slowTime) {
		GameTimer::SetTimeScale(1.0f);
	}

	if (timer_ > param_.breakTime) {
		pOwner_->SetIsBreak(true);
		BaseParticles* particle = ParticleManager::GetInstance()->CrateParticle("BossExplaode");
		particle->SetPos(pOwner_->GetPosition());
		particle->Reset();
		particle->SetLoop(false);

		pOwner_->Destroy();
	}
}

void BossStateBeDestroyed::OnExit() {
}

void BossStateBeDestroyed::Debug_Gui() {
	param_.Debug_Gui();
}

void BossStateBeDestroyed::Parameter::Debug_Gui() {
	ImGui::DragFloat("slowTime", &slowTime, 0.1f);
	ImGui::DragFloat("breakTime", &breakTime, 0.1f);
	SaveAndLoad();
}
