#include "PlayerActionShotLeft.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
// Engine
#include "Engine/Lib/GameTimer.h"

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
	clip->PoseToAnimation("left_shot", 0.2f);

	// 武器とアクションを設定する
	pWeapon_ = pOwner_->GetWeapon(PlayerWeapon::LEFT_WEAPON);
	action_ = [&] { this->StartUp(); };

	// 他のアクションを削除する(このアクションに専念させるため)
	size_t hash = typeid(PlayerActionShotLeft).hash_code();
	pManager_->DeleteOther(hash);

	// カメラアクションを実行させる
	pCameraAnimation_ = pOwner_->GetFollowCamera()->GetCameraAnimation();
	pCameraAnimation_->ExecuteShotAnimation(true);
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
	action_();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionShotLeft::OnEnd() {
	AnimationClip* clip = pOwner_->GetGameObject()->GetAnimetor()->GetAnimationClip();
	clip->PoseToAnimation("left_shotAfter", 0.2f);

	Rigidbody* rigidbody = pOwner_->GetGameObject()->GetRigidbody();
	if (rigidbody != nullptr) {
		if (rigidbody->GetGravity()) {
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
	if (pOwner_->GetWeapon(PlayerWeapon::LEFT_WEAPON)->GetIsReload()) {
		return false;
	}

	if (pInput_->IsTriggerButton(XInputButtons::L_SHOULDER)) {
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
		Vector3 dire = (pOwner_->GetTargetPos() - pOwner_->GetPosition()).Normalize();
		pOwner_->Attack(PlayerWeapon::LEFT_WEAPON, AttackContext(dire, CVector3::ZERO));
	} else {
		Vector3 dire = pOwner_->GetTransform()->srt_.rotate.MakeForward();
		pOwner_->Attack(PlayerWeapon::LEFT_WEAPON, AttackContext(dire, CVector3::ZERO));
	}
	action_ = [&] { this->Recoil(); };
	pCameraAnimation_->ExecuteShotAnimation(false);// カメラを離す

	// カメラを揺らす
	pOwner_->GetFollowCamera()->SetShake(0.2f, 1.0f);
}

void PlayerActionShotLeft::StartUp() {
	if (!pCameraAnimation_->GetShotAnimationFinish()) {
		action_ = [&] { this->Shot(); };
		actionTimer_ = 0;
	}
}

void PlayerActionShotLeft::Recoil() {
	if (!pCameraAnimation_->GetShotAnimationFinish()) {
		isFinish_ = true;
	}
}