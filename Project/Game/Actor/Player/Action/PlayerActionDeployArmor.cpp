#include "PlayerActionDeployArmor.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
#include "Engine/System/Manager/GpuParticleManager.h"
#include "Engine/Lib/Json/JsonItems.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionDeployArmor::Debug_Gui() {
	parameter_.Debug_Gui();
}

void PlayerActionDeployArmor::Parameter::Debug_Gui() {
	ImGui::DragFloat("chargeTime", &chargeTime, 01.f);
	ImGui::DragFloat3("effectOffset", &effectOffset.x, 0.1f);
	ImGui::DragFloat("cameraApproachTime", &cameraApproachTime, 0.1f);
	ImGui::DragFloat("cameraLeaveTime", &cameraLeaveTime, 0.1f);
	ImGui::DragFloat("cameraOffsetZ", &cameraOffsetZ, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionDeployArmor::Build() {
	SetName("ActionDeployArmor");
	pInput_ = Input::GetInstance();

	parameter_.SetGroupName(pManager_->GetName());
	parameter_.Load();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionDeployArmor::OnStart() {
	actionTimer_ = 0.0f;

	cameraInitOffsetZ_ = pOwner_->GetFollowCamera()->GetOffset().z;
	cameraOffsetZ_.Init(cameraInitOffsetZ_, parameter_.cameraOffsetZ, parameter_.cameraApproachTime, (int)EasingType::None::Liner, LoopType::Stop);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionDeployArmor::OnUpdate() {
	actionTimer_ += AOENGINE::GameTimer::DeltaTime();

	// カメラを動かす時間計算する
	FollowCamera* followCamera = pOwner_->GetFollowCamera();
	cameraOffsetZ_.Update(AOENGINE::GameTimer::DeltaTime());

	// Animationが終了したときの処理
	if (cameraOffsetZ_.GetIsFinish()) {
		// まだ展開をしていなければ展開して次のアニメーションを設定する
		if (!isDeploy_) {
			isDeploy_ = true;
			pOwner_->SetDeployArmor(true);
			cameraOffsetZ_.Init(parameter_.cameraOffsetZ, cameraInitOffsetZ_, parameter_.cameraLeaveTime, (int)EasingType::None::Liner, LoopType::Stop);
		}
	}

	followCamera->SetOffsetZ(cameraOffsetZ_.GetValue());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionDeployArmor::OnEnd() {
	pOwner_->SetDeployArmor(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次のアクション確認
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionDeployArmor::CheckNextAction() {
	if (cameraOffsetZ_.GetIsFinish()) {
		if (isDeploy_) {
			NextAction<PlayerActionIdle>();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerActionDeployArmor::IsInput() {
	if (pOwner_->GetIsDeployArmor()) { return false; }

	if (pInput_->IsPressButton(XInputButtons::ButtonY)) {
		return true;
	}
	return false;
}
