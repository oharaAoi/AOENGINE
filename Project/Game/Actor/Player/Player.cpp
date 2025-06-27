#include "Player.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Collision/ColliderCollector.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Scene/SceneLoader.h"
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
#include "Game/Actor/Player/Action/PlayerActionDamaged.h"
#include "Game/Actor/Player/Action/PlayerActionTurnAround.h"

Player::Player() {}
Player::~Player() {
	Finalize();
}

void Player::Finalize() {
	jet_ = nullptr;
}

void Player::Debug_Gui() {
	object_->GetTransform()->Debug_Gui();

	actionManager_->Debug_Gui();

	if (ImGui::CollapsingHeader("CurrentParameter")) {
		ImGui::DragFloat("health", &param_.health, 0.1f);
		ImGui::DragFloat("postureStability", &param_.postureStability, 0.1f);
		ImGui::DragFloat("bodyWeight", &param_.bodyWeight, 0.1f);
		ImGui::DragFloat("energy", &param_.energy, 0.1f);
		ImGui::DragFloat("energyRecoveyAmount", &param_.energyRecoveyAmount, 0.1f);
		ImGui::DragFloat("energyRecoveyCoolTime", &param_.energyRecoveyCoolTime, 0.1f);

		param_.bodyWeight = std::clamp(param_.bodyWeight, 1.0f, 100.0f);
	}

	if (ImGui::CollapsingHeader("Parameter", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("health", &initParam_.health, 0.1f);
		ImGui::DragFloat("postureStability", &initParam_.postureStability, 0.1f);
		ImGui::DragFloat("bodyWeight", &initParam_.bodyWeight, 0.1f);
		ImGui::DragFloat("energy", &initParam_.energy, 0.1f);
		ImGui::DragFloat("energyRecoveyAmount", &initParam_.energyRecoveyAmount, 0.1f);
		ImGui::DragFloat("energyRecoveyCoolTime", &initParam_.energyRecoveyCoolTime, 0.1f);

		param_.bodyWeight = std::clamp(param_.bodyWeight, 1.0f, 100.0f);

		if (ImGui::Button("Save")) {
			JsonItems::Save(GetName(), initParam_.ToJson("playerParameter"));
		}
		if (ImGui::Button("Apply")) {
			initParam_.FromJson(JsonItems::GetData(GetName(), "playerParameter"));
			param_ = initParam_;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Init() {
	SetName("Player");
	SceneLoader::Objects object = SceneLoader::GetInstance()->GetObjects("Player");

	object_ = SceneRenderer::GetInstance()->GetGameObject<BaseGameObject>("Player");
	transform_ = object_->GetTransform();

	jet_ = std::make_unique<JetEngine>();
	jet_->Init();
	jet_->SetParent(this);

	AddChild(jet_.get());

	object_->SetCollider(ColliderTags::Player::own, object.colliderType);
	ICollider* collider = object_->GetCollider();
	collider->SetSize(object.colliderSize);
	collider->SetLoacalPos(object.colliderCenter);
	collider->SetTarget(ColliderTags::Boss::own);
	collider->SetTarget(ColliderTags::Boss::missile);
	collider->SetTarget(ColliderTags::Field::ground);
	collider->SetIsStatic(false);

	// -------------------------------------------------
	// ↓ State関連
	// -------------------------------------------------
	stateMachine_ = std::make_unique<StateMachine<Player>>();
	stateMachine_->Init(this);
	stateMachine_->ChangeState<PlayerIdleState>();

	// -------------------------------------------------
	// ↓ Action関連
	// -------------------------------------------------

	actionManager_ = std::make_unique<ActionManager<Player>>();
	actionManager_->Init(this, "PlayerAction");
	actionManager_->BuildAction<PlayerActionIdle>();
	actionManager_->BuildAction<PlayerActionMove>();
	actionManager_->BuildAction<PlayerActionJump>();
	actionManager_->BuildAction<PlayerActionQuickBoost>();
	actionManager_->BuildAction<PlayerActionBoost>();
	actionManager_->BuildAction<PlayerActionShotRight>();
	actionManager_->BuildAction<PlayerActionShotLeft>();
	actionManager_->BuildAction<PlayerActionDamaged>();
	actionManager_->BuildAction<PlayerActionTurnAround>();

	size_t hash = typeid(PlayerActionIdle).hash_code();
	actionManager_->AddRunAction(hash);

	// -------------------------------------------------
	// ↓ Parameter関連
	// -------------------------------------------------
	isKnockback_ = false;

	isLanding_ = false;
	isMoving_ = false;

	initParam_.FromJson(JsonItems::GetData(GetName(), "playerParameter"));
	param_ = initParam_;
	param_.postureStability -= initParam_.postureStability;
	
#ifdef _DEBUG
	EditorWindows::AddObjectWindow(this, GetName());
#endif // _DEBUG

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Update() {
	actionManager_->Update();
	stateMachine_->Update();
	IsBoostMode();

	if (reticle_->GetLockOn()) {
		Vector3 toTarget = reticle_->GetTargetPos() - transform_->translate_;
		toTarget.y = 0.0f; // Y軸の高さ成分を無視して水平方向ベクトルに
		toTarget = toTarget.Normalize();
		Quaternion targetToRotate = Quaternion::LookRotation(toTarget);
		transform_->rotation_ = Quaternion::Slerp(transform_->rotation_, targetToRotate, 0.9f);
	}

	jet_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Draw() const {
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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ ノックバック処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Knockback(const Vector3& direction) {
	if (isKnockback_) { return; }
	Vector3 dire = direction;

	// playerが地面についている際はy軸方向にノックバックしない用に対策する
	if (isLanding_) {
		dire.y = 0.f;
		dire = dire.Normalize();
	}

	// ノックバックさせる
	isKnockback_ = true;
	stateMachine_->ChangeState<PlayerKnockbackState>();

	size_t hash = typeid(PlayerActionDamaged).hash_code();
	actionManager_->ChangeAction(hash);

	knockBackDire_ = dire;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ エネルギーを回復する
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::RecoveryEN(float timer) {
	if (isLanding_) {
		if (timer > param_.energyRecoveyCoolTime) {
			param_.energy += param_.energyRecoveyAmount * GameTimer::DeltaTime();
			param_.energy = std::clamp(param_.energy, 0.0f, initParam_.energy);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ エネルギーを消費する
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::ConsumeEN(float cousumeAmount) {
	param_.energy -= cousumeAmount;
	param_.energy = std::clamp(param_.energy, 0.0f, initParam_.energy);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Landing() {
	isLanding_ = true;
	jet_->JetIsStop();
}

void Player::IsBoostMode() {
	if (Input::GetInstance()->GetIsPadTrigger(XInputButtons::BUTTON_B)) {
		jet_->SetIsBoostMode();
	}
}
