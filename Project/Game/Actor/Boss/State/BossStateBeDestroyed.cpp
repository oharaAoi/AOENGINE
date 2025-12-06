#include "BossStateBeDestroyed.h"
#include "Game/Actor/Boss/Boss.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Manager/ParticleManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateBeDestroyed::OnStart() {
	SetName("BossStateBeDestroyed");

	param_.SetGroupName(stateMachine_->GetName());
	param_.Load();

	timer_ = 0.0f;
	AOENGINE::GameTimer::SetTimeScale(param_.slowScale);

	preRotate_ = pOwner_->GetTransform()->srt_.rotate;
	
	Math::Quaternion pitchRotate = Math::Quaternion::AngleAxis(-45.0f * kToRadian, CVector3::RIGHT);
	targetRotate_ = preRotate_ * pitchRotate;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateBeDestroyed::OnUpdate() {
	timer_ += AOENGINE::GameTimer::DeltaTime();

	// 時間の計測
	float t = timer_ / param_.slowTime;
	t = std::clamp(t, 0.0f, param_.slowTime);
	pOwner_->GetTransform()->srt_.rotate = Math::Quaternion::Slerp(preRotate_, targetRotate_, t);

	// スロウを基に戻す
	if (timer_ > param_.slowTime) {
		AOENGINE::GameTimer::SetTimeScale(1.0f);
	}

	// 破壊particleを出す
	if (timer_ > param_.breakTime) {
		pOwner_->SetIsBreak(true);
		BaseParticles* particle = ParticleManager::GetInstance()->CrateParticle("BossExplaode");
		particle->SetPos(pOwner_->GetPosition());
		particle->Reset();
		particle->SetLoop(false);

		pOwner_->Destroy();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateBeDestroyed::OnExit() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateBeDestroyed::Debug_Gui() {
	param_.Debug_Gui();
}

void BossStateBeDestroyed::Parameter::Debug_Gui() {
	ImGui::DragFloat("slowTime", &slowTime, 0.1f);
	ImGui::DragFloat("breakTime", &breakTime, 0.1f);
	ImGui::DragFloat("slowScale", &slowScale, 0.1f);
	SaveAndLoad();
}
