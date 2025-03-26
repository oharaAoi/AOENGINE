#include "Player.h"
#include "Game/Actor/Player/State/PlayerIdleState.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
#include "Game/Actor/Player/Action/PlayerActionMove.h"
#include "Game/Actor/Player/Action/PlayerActionJump.h"

Player::Player() {}
Player::~Player() {}

void Player::Finalize() {
}

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

	actionManager_.Init(this);
	actionManager_.BuildAction<PlayerActionIdle>();
	actionManager_.BuildAction<PlayerActionMove>();
	actionManager_.BuildAction<PlayerActionJump>();

	size_t hash = typeid(PlayerActionMove).hash_code();
	actionManager_.AddRunAction(hash);

}

void Player::Update() {
	actionManager_.Update();
	stateMachine_->Update();

	BaseGameObject::Update();
}

void Player::Draw() const {
	BaseGameObject::Draw();
}

#ifdef _DEBUG
void Player::Debug_Gui() {

}
#endif // _DEBUG