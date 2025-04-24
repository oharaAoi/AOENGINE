#include "Player.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Game/Actor/Player/State/PlayerIdleState.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
#include "Game/Actor/Player/Action/PlayerActionMove.h"
#include "Game/Actor/Player/Action/PlayerActionJump.h"
#include "Game/Actor/Player/Action/PlayerActionQuickBoost.h"
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

	jet_ = std::make_unique<BaseGameObject>();
	jet_->Init();
	jet_->GetTransform()->SetParent(this->GetTransform()->GetWorldMatrix());
	jet_->GetTransform()->translate_ = Vector3{ 0.0f, 1.0f, -0.6f };

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
	actionManager_.BuildAction<PlayerActionQuickBoost>();
	actionManager_.BuildAction<PlayerActionShotRight>();

	size_t hash = typeid(PlayerActionMove).hash_code();
	actionManager_.AddRunAction(hash);

	// -------------------------------------------------
	// ↓ Effect関連
	// -------------------------------------------------
	gunFireParticles_ = std::make_unique<GunFireParticles>();
	gunFireParticles_->Init("gunFireParticles");
	gunFireParticles_->SetParent(this->GetTransform()->GetWorldMatrix());

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
		//Vector3 toDirection = reticle_->GetTargetPos() - transform_->translate_;
		Quaternion targetToRotate = Quaternion::LookAt(transform_->translate_, reticle_->GetTargetPos());
		transform_->rotation_ = Quaternion::Slerp(transform_->rotation_, targetToRotate, 0.9f);
	}

	BaseGameObject::Update();

	jet_->Update();
	gunFireParticles_->Update(Render::GetCameraRotate());
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

	// effectを出す
	Vector3 pos = transform_->rotation_.MakeForward() * 1.4f;
	pos.y += 1.0f;
	gunFireParticles_->SetPos(pos);
	gunFireParticles_->SetOnShot();
}