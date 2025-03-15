#include "Player.h"
#include "Game/Actor/Player/State/PlayerIdleState.h"

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

}

void Player::Update() {
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