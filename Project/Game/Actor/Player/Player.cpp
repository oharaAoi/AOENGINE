#include "Player.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Collision/ColliderCollector.h"
#include "Game/Information/ColliderCategory.h"
#include "Game/Actor/Player/State/PlayerIdleState.h"
#include "Game/Actor/Player/State/PlayerKnockbackState.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
#include "Game/Actor/Player/Action/PlayerActionMove.h"
#include "Game/Actor/Player/Action/PlayerActionJump.h"
#include "Game/Actor/Player/Action/PlayerActionQuickBoost.h"
#include "Game/Actor/Player/Action/PlayerActionBoost.h"
#include "Game/Actor/Player/Action/PlayerActionShotRight.h"
#include "Game/Actor/Player/Action/PlayerActionShotLeft.h"

Player::Player() {}
Player::~Player() {
	Finalize();
}

void Player::Finalize() {
	jet_ = nullptr;
}

#ifdef _DEBUG
void Player::Debug_Gui() {
	transform_->Debug_Gui();

	collider_->Debug_Gui();

	if (ImGui::CollapsingHeader("CurrentParameter", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("energy", &param_.energy, 0.1f);
	}

	if (ImGui::CollapsingHeader("Parameter", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("energy", &initParam_.energy, 0.1f);

		if (ImGui::Button("Save")) {
			JsonItems::Save(GetName(), initParam_.ToJson("playerParameter"));
		}
		if (ImGui::Button("Apply")) {
			initParam_.FromJson(JsonItems::GetData(GetName(), "playerParameter"));
			param_ = initParam_;
		}
	}
}

#endif // _DEBUG

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Init() {
	BaseGameObject::Init();
	SetObject("player.obj");
	SetName("Player");

	jet_ = std::make_unique<JetEngine>();
	jet_->Init();
	jet_->SetParent(this);

	AddChild(jet_.get());

	SetCollider(ColliderTags::Player::own, ColliderShape::SPHERE);
	collider_->SetTarget(ColliderTags::Boss::own);
	collider_->SetTarget(ColliderTags::Field::ground);
	collider_->SetRadius(1.7f);
	collider_->SetIsStatic(false);

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
	actionManager_.BuildAction<PlayerActionBoost>();
	actionManager_.BuildAction<PlayerActionShotRight>();
	actionManager_.BuildAction<PlayerActionShotLeft>();

	size_t hash = typeid(PlayerActionMove).hash_code();
	actionManager_.AddRunAction(hash);

	// -------------------------------------------------
	// ↓ Parameter関連
	// -------------------------------------------------
	isKnockback_ = false;

	initParam_.FromJson(JsonItems::GetData(GetName(), "playerParameter"));
	param_ = initParam_;

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, GetName());
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
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Draw() const {
	BaseGameObject::Draw();
	jet_->Draw();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 武器をセットする
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::SetWeapon(BaseWeapon* _weapon, PlayerWeapon type) {
	if (type == PlayerWeapon::LEFT_WEAPON) {
		pWeapons_[LEFT_WEAPON] = _weapon;
	} else if (type == PlayerWeapon::RIGHT_WEAPON) {
		pWeapons_[RIGHT_WEAPON] = _weapon;
	}
}

void Player::Knockback() {
	isKnockback_ = true;
	stateMachine_->ChangeState<PlayerKnockbackState>();
}

void Player::Landing() {
	size_t hash = typeid(PlayerActionIdle).hash_code();
	actionManager_.ChangeAction(hash);
}