#include "PlayerKnockbackState.h"
#include "Engine/Lib/GameTimer.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/State/PlayerIdleState.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
#include "PlayerDeadState.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerKnockbackState::Debug_Gui() {
	param_.Debug_Gui();
}

void PlayerKnockbackState::Parameter::Debug_Gui() {
	ImGui::DragFloat("knockbackTime", &knockbackTime, 0.1f);
	ImGui::DragFloat("knockStrength", &knockStrength, 0.1f);
	ImGui::DragFloat("knockDecay", &knockDecay, 0.1f);
	ImGui::DragFloat("glitchNoiseTime", &glitchNoiseTime, 0.1f);
	ImGui::DragFloat("glitchNoiseStrength", &glitchNoiseStrength, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 開始処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerKnockbackState::OnStart() {
	param_.Load();
	timer_ = 0.0f;

	glitchNoise_ = Engine::GetPostProcess()->GetEffectAs<PostEffect::GlitchNoise>(PostEffectType::GlitchNoise);
	glitchNoise_->StartNoise(param_.glitchNoiseStrength, param_.glitchNoiseTime);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerKnockbackState::OnUpdate() {
	Knockback();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerKnockbackState::OnExit() {
	pOwner_->SetKnockback(false);

	size_t hash = typeid(PlayerActionIdle).hash_code();
	pOwner_->GetActionManager()->ChangeAction(hash);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ ノックバック処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerKnockbackState::Knockback() {
	timer_ += AOENGINE::GameTimer::DeltaTime();
	param_.knockStrength *= param_.knockDecay;

	// ノックバック方向のベクトルを取得
	Math::Vector3 direction = pOwner_->GetKnockBackDire();
	acceleration_ = (direction * param_.knockStrength) * AOENGINE::GameTimer::DeltaTime();
	velocity_ += acceleration_ * AOENGINE::GameTimer::DeltaTime();
	pOwner_->GetTransform()->srt_.translate += velocity_;

	if (timer_ > param_.knockbackTime) {
		stateMachine_->ChangeState<PlayerIdleState>();
		pOwner_->SetPostureStability(0.0f);
	}
}
