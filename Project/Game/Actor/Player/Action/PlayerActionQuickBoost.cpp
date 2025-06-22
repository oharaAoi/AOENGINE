#include "PlayerActionQuickBoost.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionMove.h"
#include "Engine/System/Manager/ParticleManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::Debug_Gui() {
	ImGui::DragFloat("first_boostForce", &initParam_.boostForce, 0.1f);
	ImGui::DragFloat("first_decelerationRaito", &initParam_.decelerationRaito, 0.01f);
	ImGui::DragFloat("first_boostEnergy", &initParam_.boostEnergy, 0.01f);
	ImGui::DragFloat("cameraShakeTime", &initParam_.cameraShakeTime, 0.1f);
	ImGui::DragFloat("cameraShakeStrength", &initParam_.cameraShakeStrength, 0.1f);

	ImGui::Text("boostForce: (%.2f)", param_.boostForce);
	ImGui::Text("decelerationRaito: (%.2f)", param_.decelerationRaito);
	ImGui::Text("boostEnergy: (%.2f)", param_.boostEnergy);

	if (ImGui::Button("Save")) {
		JsonItems::Save(pManager_->GetName(), initParam_.ToJson(param_.GetName()));
	}
	if (ImGui::Button("Apply")) {
		initParam_.FromJson(JsonItems::GetData(pManager_->GetName(), param_.GetName()));
		param_ = initParam_;
	}

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

	initParam_.FromJson(JsonItems::GetData(pManager_->GetName(), param_.GetName()));
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
	boostParticle_->Update();
	Boost();
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
	if (pInput_->GetIsPadTrigger(XInputButtons::BUTTON_X)) {
		return true;
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
	pOwnerTransform_->translate_ += velocity_;

	// 減速計算する
	param_.boostForce *= param_.decelerationRaito;
}