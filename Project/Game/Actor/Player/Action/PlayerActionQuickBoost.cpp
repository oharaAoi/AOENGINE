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
	float t = actionTimer_ / param_.decelerationTime;
	float bezierValue = param_.decelerationCurve.BezierValue(1 - t);
	ImGui::Text("bezierValue: (%.2f)", bezierValue);
	initParam_.Debug_Gui();
}

void PlayerActionQuickBoost::Parameter::Debug_Gui() {
	ImGui::DragFloat("boostForce", &boostForce, 0.1f);
	ImGui::DragFloat("decelerationRaito", &decelerationRaito, 0.01f);
	ImGui::DragFloat("boostEnergy", &boostEnergy, 0.01f);
	ImGui::DragFloat("cameraShakeTime", &cameraShakeTime, 0.1f);
	ImGui::DragFloat("cameraShakeStrength", &cameraShakeStrength, 0.1f);
	ImGui::DragFloat("decelerationTime", &decelerationTime, 0.1f);
	decelerationCurve.Debug_Gui();
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

	pRigidBody_ = pOwner_->GetGameObject()->GetRigidbody();

	pRadialBlur_ = Engine::GetPostProcess()->GetRadialBlur();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::OnStart() {
	param_ = initParam_;

	// 初速度を求める
	stick_ = pInput_->GetLeftJoyStick().Normalize();
	direction_ = pOwner_->GetFollowCamera()->GetAngleX().Rotate(Vector3{ stick_.x, 0.0f, stick_.y });

	acceleration_ = direction_ * param_.boostForce;
	pRigidBody_->SetVelocity(acceleration_ * GameTimer::DeltaTime());

	// エネルギーを消費する
 	Player::Parameter& ownerParam_ = pOwner_->GetParam();
	ownerParam_.energy -= param_.boostEnergy;


	boostParticle_->Reset();

	actionTimer_ = 0;

	pManager_->DeleteAction(typeid(PlayerActionMove).hash_code());
	pInput_->Vibrate(1.0f, 0.2f);

	// カメラを揺らす
	pOwner_->GetFollowCamera()->SetShake(0.2f, 0.6f);
	// ブラーを画面にかける
	pRadialBlur_->Start(0.2f, 0.2f, false);
	// boostをonにする
	pOwner_->GetJetEngine()->BoostOn();
	pOwner_->GetJetEngine()->JetIsStart();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::OnUpdate() {
	actionTimer_ += GameTimer::DeltaTime();

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
	if (actionTimer_ >= param_.decelerationTime) {
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
	float t = actionTimer_ / param_.decelerationTime;
	float bezierValue = param_.decelerationCurve.BezierValue(1 - t);

	param_.boostForce *= bezierValue;
	acceleration_ = direction_ * (initParam_.boostForce * bezierValue);

	pRigidBody_->AddVelocity(acceleration_ * GameTimer::DeltaTime());
}

void PlayerActionQuickBoost::Deceleration() {

}
