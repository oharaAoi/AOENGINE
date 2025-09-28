#include "PlayerActionQuickBoost.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionMove.h"
#include "Engine/System/Manager/ParticleManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::Debug_Gui() {
	ImGui::Text("boostForce: (%.2f)", param_.boostForce);
	ImGui::Text("decelerationRaito: (%.2f)", param_.decelerationRaito);
	ImGui::Text("boostEnergy: (%.2f)", param_.boostEnergy);
	param_.Debug_Gui();
}

void PlayerActionQuickBoost::Parameter::Debug_Gui() {
	ImGui::DragFloat("first_boostForce", &boostForce, 0.1f);
	ImGui::DragFloat("first_decelerationRaito", &decelerationRaito, 0.01f);
	ImGui::DragFloat("first_boostEnergy", &boostEnergy, 0.01f);
	ImGui::DragFloat("cameraShakeTime", &cameraShakeTime, 0.1f);
	ImGui::DragFloat("cameraShakeStrength", &cameraShakeStrength, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::Build() {
	SetName("actionQuickBoost");
	pInput_ = Input::GetInstance();
	pOwnerTransform_ = pOwner_->GetTransform();

	boostParticle_ = ParticleManager::GetInstance()->CrateParticle("QuickBoost");
	boostParticle_->SetParent(pOwner_->GetJet()->GetTransform()->GetWorldMatrix());
	boostParticle_->SetIsStop(true);
	boostParticle_->SetLoop(false);

	initParam_.SetGroupName(pManager_->GetName());
	initParam_.Load();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::OnStart() {
	param_ = initParam_;

	// 初速度を求める
	stick_ = pInput_->GetLeftJoyStick();
	direction_ = pOwner_->GetFollowCamera()->GetAngleX().Rotate(Vector3{ stick_.x, 0.0f, stick_.y });

	acceleration_ = direction_ * param_.boostForce;
	velocity_ = { 0.0f, 0.0f, 0.0f };

	// エネルギーを消費する
	Player::Parameter& ownerParam_ = pOwner_->GetParam();
	ownerParam_.energy -= param_.boostEnergy;

	//pOwner_->GetFollowCamera()->SetShake(initParam_.cameraShakeTime, initParam_.cameraShakeStrength);
	pOwner_->GetJetEngine()->JetIsStart();

	boostParticle_->Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::OnUpdate() {
	Boost();
	pOwner_->UpdateJoint();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::OnEnd() {

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次に行うアクションの判定
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::CheckNextAction() {
	if (param_.boostForce <= 0.1f) {
		NextAction<PlayerActionMove>();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerActionQuickBoost::IsInput() {
	if (pInput_->IsTriggerButton(XInputButtons::BUTTON_X)) {
		if (pOwner_->GetParam().energy > 0.0f) {
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ main action
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::Boost() {
	pOwner_->GetJetEngine()->JetIsStart();
	stick_ = pInput_->GetLeftJoyStick();
	direction_ = pOwner_->GetFollowCamera()->GetAngleX().Rotate(Vector3{ stick_.x, 0.0f, stick_.y });
	acceleration_ = direction_ * param_.boostForce;

	velocity_ += acceleration_ * GameTimer::DeltaTime();
	pOwnerTransform_->srt_.translate += velocity_;

	// 減速計算する
	param_.boostForce *= param_.decelerationRaito;
}