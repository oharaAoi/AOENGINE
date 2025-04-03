#include "Player.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Game/Actor/Player/State/PlayerIdleState.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
#include "Game/Actor/Player/Action/PlayerActionMove.h"
#include "Game/Actor/Player/Action/PlayerActionJump.h"
#include "Game/Actor/Player/Action/PlayerActionShotRight.h"

Player::Player() {}
Player::~Player() {}

void Player::Finalize() {
}

#ifdef _DEBUG
void Player::Debug_Gui() {
	transform_->Debug_Gui();
}
#endif // _DEBUG

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Init() {
	BaseGameObject::Init();
	SetObject("player.obj");

	// -------------------------------------------------
	// ↓ State関連
	// -------------------------------------------------
	stateMachine_ = std::make_unique<StateMachine<Player>>();
	stateMachine_->Init(this);
	stateMachine_->ChangeState<PlayerIdleState>();

	// -------------------------------------------------
	// ↓ Action関連
	// -------------------------------------------------

	actionManager_.Init(this, "playerAction");
	actionManager_.BuildAction<PlayerActionIdle>();
	actionManager_.BuildAction<PlayerActionMove>();
	actionManager_.BuildAction<PlayerActionJump>();
	actionManager_.BuildAction<PlayerActionShotRight>();

	size_t hash = typeid(PlayerActionMove).hash_code();
	actionManager_.AddRunAction(hash);

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, "player");
#endif // _DEBUG

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Update() {
	actionManager_.Update();
	stateMachine_->Update();

	if (reticle_->GetLockOn()) {
		Vector3 toDirection = reticle_->GetTargetPos() - transform_->translate_;
		Quaternion targetToRotate = Quaternion::LookAt(transform_->rotation_.MakeForward(), toDirection);
		transform_->rotation_ = Quaternion::Slerp(transform_->rotation_,  targetToRotate, 0.1f);
	}

	BaseGameObject::Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Draw() const {
	BaseGameObject::Draw();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 弾を打つ
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Shot(float speed) {
	pBulletManager_->AddBullet(transform_->translate_, transform_->rotation_.MakeForward() * speed);
}