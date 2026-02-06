#include "BossStateStun.h"
#include "Engine/System/Audio/AudioPlayer.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/State/BossStateNormal.h"
#include "Game/UI/Boss/BossUIs.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateStun::OnStart() {
	SetName("StunState");

	stateTime_ = 0.0f;
	slowTime_ = 0.0f;
	param_.SetGroupName(stateMachine_->GetName());
	param_.Load();

	pOwner_->GetGameObject()->GetRigidbody()->SetGravity(true);

	pOwner_->SetIsStun(true);
	pOwner_->SetExecute(false);

	AOENGINE::Rigidbody* rigid = pOwner_->GetGameObject()->GetRigidbody();
	rigid->SetVelocity(CVector3::ZERO);

	// ----------------------
	// ↓ 演出関連
	// ----------------------

	effect_ = AOENGINE::ParticleManager::GetInstance()->CreateParticle("BossStunEffect");
	effect_->SetParent(pOwner_->GetTransform()->GetWorldMatrix());

	AOENGINE::GameTimer::SetTimeScale(0.1f);

	AOENGINE::AudioPlayer::SinglShotPlay("stan_se.mp3", 0.5f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateStun::OnUpdate() {
	stateTime_ += AOENGINE::GameTimer::DeltaTime();
	slowTime_ += AOENGINE::GameTimer::FixedDeltaTime();
	pOwner_->SetStunRemainingTime(stateTime_ / param_.stanTime);

	// エフェクトが出し終わっていたら別の位置に移動させてもう一度射出する
	if (effect_->GetIsStop()) {
		Math::Vector3 newPos = RandomVector3(CVector3::UNIT * -param_.effectRandDistance, CVector3::UNIT * param_.effectRandDistance);
		effect_->Reset();
		effect_->SetPos(newPos);
	}

	// ヒットスロウ
	if (slowTime_ >= param_.stanSlowTime) {
		AOENGINE::GameTimer::SetTimeScale(1.0f);
	}

	// スタンの終了
	if (stateTime_ >= param_.stanTime) {
		stateMachine_->ChangeState<BossStateNormal>();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateStun::OnExit() {
	pOwner_->GetGameObject()->GetRigidbody()->SetGravity(false);

	pOwner_->ResetStun();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateStun::Debug_Gui() {
	param_.Debug_Gui();
}

void BossStateStun::Parameter::Debug_Gui() {
	ImGui::DragFloat("stanTime", &stanTime, 0.1f);
	ImGui::DragFloat("stanSlowTime", &stanSlowTime, 0.1f);
	ImGui::DragFloat("effectRandDistance", &effectRandDistance, 0.1f);
	SaveAndLoad();
}