#include "PlayerActionShotLeft.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
// Engine
#include "Engine/Lib/GameTimer.h"
#include "Engine/System/Audio/AudioPlayer.h"

void PlayerActionShotLeft::Debug_Gui() {
	param_.Debug_Gui();
}

void PlayerActionShotLeft::Parameter::Debug_Gui() {
	ImGui::DragFloat("animationTime", &animationTime);
	ImGui::DragFloat("cameraShakeTime", &cameraShakeTime);
	ImGui::DragFloat("cameraShakeStrength", &cameraShakeStrength);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionShotLeft::Build() {
	SetName("actionShotLeft");
	pInput_ = Input::GetInstance();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionShotLeft::OnStart() {
	notShotTimer_ = 0.0f;

	// playerのAnimationを変更する
	AnimationClip* clip = pOwner_->GetGameObject()->GetAnimetor()->GetAnimationClip();
	clip->PoseToAnimation("left_shot", param_.animationTime);

	// 武器とアクションを設定する
	pWeapon_ = pOwner_->GetWeapon(PlayerWeapon::Left_Weapon);
	action_ = [&] { this->StartUp(); };

	// 他のアクションを削除する(このアクションに専念させるため)
	size_t hash = typeid(PlayerActionShotLeft).hash_code();
	pManager_->DeleteOther(hash);

	// カメラアクションを実行させる
	pCameraAnimation_ = pOwner_->GetFollowCamera()->GetCameraAnimation("shotAnimation");
	pCameraAnimation_->CallExecute(true);
	isFinish_ = false;

	// 重力が適応されている場合は重力を無効にする
	Rigidbody* rigidbody = pOwner_->GetGameObject()->GetRigidbody();
	if (rigidbody != nullptr) {
		rigidbody->SetGravity(false);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionShotLeft::OnUpdate() {
	Rigidbody* rigidbody = pOwner_->GetGameObject()->GetRigidbody();
	if (rigidbody != nullptr) {
		rigidbody->SetGravity(false);
	}

	// main
	action_();

	// ----------------------
	// 向き更新
	// ----------------------
	pOwner_->LookTarget(1.0f, pOwner_->GetReticle()->GetLockOn());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionShotLeft::OnEnd() {
	AnimationClip* clip = pOwner_->GetGameObject()->GetAnimetor()->GetAnimationClip();
	clip->PoseToAnimation("left_shotAfter", param_.animationTime);

	Rigidbody* rigidbody = pOwner_->GetGameObject()->GetRigidbody();
	if (rigidbody != nullptr) {
		if (!rigidbody->GetGravity()) {
			rigidbody->SetGravity(true);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次に行うアクションの判定
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionShotLeft::CheckNextAction() {
	// ボタンを押して入なかったら待機状態に行く
	if (isFinish_) {
		NextAction<PlayerActionIdle>();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerActionShotLeft::IsInput() {
	if (pOwner_->GetWeapon(PlayerWeapon::Left_Weapon)->GetIsReload()) {
		return false;
	}

	if (pInput_->IsTriggerButton(XInputButtons::LShoulder)) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ main action
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionShotLeft::Shot() {
	// shotを放つ
	if (pOwner_->GetIsLockOn()) {
		Math::Vector3 dire = (pOwner_->GetTargetPos() - pOwner_->GetPosition()).Normalize();
		pOwner_->Attack(PlayerWeapon::Left_Weapon, AttackContext(dire, pOwner_->GetTargetPos()));
	} else {
		Math::Vector3 dire = pOwner_->GetTransform()->srt_.rotate.MakeForward();
		pOwner_->Attack(PlayerWeapon::Left_Weapon, AttackContext(dire, CVector3::ZERO));
	}
	action_ = [&] { this->Recoil(); };
	pCameraAnimation_->CallExecute(false);// カメラを離す

	// カメラを揺らす
	pOwner_->GetFollowCamera()->SetShake(param_.cameraShakeTime, param_.cameraShakeStrength);
}

void PlayerActionShotLeft::StartUp() {
	if (pCameraAnimation_->GetIsFinish()) {
		action_ = [&] { this->Shot(); };
		actionTimer_ = 0;
	}
}

void PlayerActionShotLeft::Recoil() {
	if (pCameraAnimation_->GetIsFinish()) {
		isFinish_ = true;
	}
}