#include "BossStateStan.h"
#include "Engine/System/Audio/AudioPlayer.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/State/BossStateNormal.h"
#include "Game/UI/Boss/BossUIs.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateStan::OnStart() {
	SetName("StanState");

	stateTime_ = 0.0f;
	slowTime_ = 0.0f;
	param_.SetGroupName(stateMachine_->GetName());
	param_.Load();

	pOwner_->GetGameObject()->GetRigidbody()->SetGravity(true);

	pOwner_->SetIsStan(true);
	pOwner_->SetExecute(false);

	// ----------------------
	// ↓ 演出関連
	// ----------------------

	effect_ = ParticleManager::GetInstance()->CrateParticle("BossStanEffect");
	effect_->SetParent(pOwner_->GetTransform()->GetWorldMatrix());

	GameTimer::SetTimeScale(0.1f);

	AudioPlayer::SinglShotPlay("stan_se.mp3", 0.5f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateStan::OnUpdate() {
	stateTime_ += GameTimer::DeltaTime();
	slowTime_ += GameTimer::FixedDeltaTime();
	pOwner_->SetStanRemainingTime(stateTime_ / param_.stanTime);

	// エフェクトが出し終わっていたら別の位置に移動させてもう一度射出する
	if (effect_->GetIsStop()) {
		Math::Vector3 newPos = RandomVector3(CVector3::UNIT * -param_.effectRandDistance, CVector3::UNIT * param_.effectRandDistance);
		effect_->Reset();
		effect_->SetPos(newPos);
	}

	// ヒットスロウ
	if (slowTime_ >= param_.stanSlowTime) {
		GameTimer::SetTimeScale(1.0f);
	}

	// スタンの終了
	if (stateTime_ >= param_.stanTime) {
		stateMachine_->ChangeState<BossStateNormal>();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateStan::OnExit() {
	pOwner_->GetGameObject()->GetRigidbody()->SetGravity(false);

	pOwner_->ResetStan();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateStan::Debug_Gui() {
	param_.Debug_Gui();
}

void BossStateStan::Parameter::Debug_Gui() {
	ImGui::DragFloat("stanTime", &stanTime, 0.1f);
	ImGui::DragFloat("stanSlowTime", &stanSlowTime, 0.1f);
	ImGui::DragFloat("effectRandDistance", &effectRandDistance, 0.1f);
	SaveAndLoad();
}