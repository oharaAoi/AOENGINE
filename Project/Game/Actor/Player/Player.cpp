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
#include "Game/Actor/Player/Action/PlayerActionRightShoulder.h"
#include "Game/Actor/Player/Action/PlayerActionDamaged.h"
#include "Game/Actor/Player/Action/PlayerActionTurnAround.h"
#include "Game/Actor/Player/Action/PlayerActionDeployArmor.h"

Player::Player() {}
Player::~Player() {
	Finalize();
}

void Player::Finalize() {
	jet_ = nullptr;
}

void Player::Debug_Gui() {
	object_->Debug_Gui();
	
	actionManager_->Debug_Gui();

	if (ImGui::CollapsingHeader("LegCollider")) {
		legCollider_->Debug_Gui();
	}

	if (ImGui::CollapsingHeader("CurrentParameter")) {
		ImGui::DragFloat("health", &param_.health, 0.1f);
		ImGui::DragFloat("postureStability", &param_.postureStability, 0.1f);
		ImGui::DragFloat("bodyWeight", &param_.bodyWeight, 0.1f);
		ImGui::DragFloat("energy", &param_.energy, 0.1f);
		ImGui::DragFloat("energyRecoveyAmount", &param_.energyRecoveyAmount, 0.1f);
		ImGui::DragFloat("energyRecoveyCoolTime", &param_.energyRecoveyCoolTime, 0.1f);

		param_.bodyWeight = std::clamp(param_.bodyWeight, 1.0f, 100.0f);
	}

	if (ImGui::CollapsingHeader("Parameter")) {
		ImGui::DragFloat("health", &initParam_.health, 0.1f);
		ImGui::DragFloat("postureStability", &initParam_.postureStability, 0.1f);
		ImGui::DragFloat("bodyWeight", &initParam_.bodyWeight, 0.1f);
		ImGui::DragFloat("energy", &initParam_.energy, 0.1f);
		ImGui::DragFloat("energyRecoveyAmount", &initParam_.energyRecoveyAmount, 0.1f);
		ImGui::DragFloat("energyRecoveyCoolTime", &initParam_.energyRecoveyCoolTime, 0.1f);

		ImGui::DragFloat("legColliderRadius", &initParam_.legColliderRadius, 0.1f);
		ImGui::DragFloat("legColliderPosY", &initParam_.legColliderPosY, 0.1f);

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
	initParam_.FromJson(JsonItems::GetData(GetName(), "playerParameter"));
	param_ = initParam_;

	SceneLoader::Objects object = SceneLoader::GetInstance()->GetObjects("Player");

	object_ = SceneRenderer::GetInstance()->GetGameObject<BaseGameObject>("Player");
	transform_ = object_->GetTransform();
	object_->SetAnimater("./Game/Assets/Load/Models/Player/", "player.gltf", true, true, false);
	object_->GetAnimetor()->GetAnimationClip()->SetIsLoop(false);

	jet_ = std::make_unique<JetEngine>();
	jet_->Init();
	jet_->SetParent(this);

	AddChild(jet_.get()); 

	ICollider* collider = object_->GetCollider("player");
	collider->SetIsStatic(false);

	ICollider* colliderLeg = object_->GetCollider("playerLeg");
	colliderLeg->SetOnCollision([this](ICollider* other) { LegOnCollision(other); });

	object_->SetPhysics();

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
	actionManager_->BuildAction<PlayerActionRightShoulder>();
	actionManager_->BuildAction<PlayerActionDamaged>();
	actionManager_->BuildAction<PlayerActionTurnAround>();
	actionManager_->BuildAction<PlayerActionDeployArmor>();

	size_t hash = typeid(PlayerActionIdle).hash_code();
	actionManager_->AddRunAction(hash);

	// -------------------------------------------------
	// ↓ Parameter関連
	// -------------------------------------------------
	isKnockback_ = false;

	isLanding_ = false;
	isMoving_ = false;

	deployArmor_ = false;

	param_.postureStability -= initParam_.postureStability;

	Skeleton* skeleton = object_->GetAnimetor()->GetSkeleton();
	leftHandMat_ = skeleton->GetSkeltonSpaceMat("left_hand") * transform_->GetWorldMatrix();
	rightHandMat_ = skeleton->GetSkeltonSpaceMat("right_hand") * transform_->GetWorldMatrix();
	rightShoulderMat_ = skeleton->GetSkeltonSpaceMat("right_shoulder") * transform_->GetWorldMatrix();
	
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

	transform_->Update();
	Skeleton* skeleton = object_->GetAnimetor()->GetSkeleton();
	leftHandMat_ = skeleton->GetSkeltonSpaceMat("left_hand") * transform_->GetWorldMatrix();
	rightHandMat_ = skeleton->GetSkeltonSpaceMat("right_hand") * transform_->GetWorldMatrix();
	rightShoulderMat_ = skeleton->GetSkeltonSpaceMat("right_shoulder") * transform_->GetWorldMatrix();

	CameraIncline();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Draw() const {
	//object_->GetAnimetor()->GetSkeleton()->DrawBone(transform_->GetWorldMatrix());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 武器をセットする
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::SetWeapon(BaseWeapon* _weapon, PlayerWeapon type) {
	if (type == PlayerWeapon::LEFT_WEAPON) {
		pWeapons_[LEFT_WEAPON] = _weapon;
	} else if (type == PlayerWeapon::RIGHT_WEAPON) {
		pWeapons_[RIGHT_WEAPON] = _weapon;
	} else if (type == PlayerWeapon::RIGHT_SHOULDER) {
		pWeapons_[PlayerWeapon::RIGHT_SHOULDER] = _weapon;
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
// ↓ 着地時の処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Landing() {
	isLanding_ = true;
	jet_->JetIsStop();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ BoostをOnにする
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::IsBoostMode() {
	if (Input::GetInstance()->IsTriggerButton(XInputButtons::BUTTON_B)) {
		jet_->SetIsBoostMode();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 足のCollider
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::LegOnCollision([[maybe_unused]] ICollider* other) {
	if (other->GetCategoryName() == "building" || other->GetCategoryName() == "ground") {
		Landing();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ カメラを傾ける
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::CameraIncline() {
	// スクリーン座標系でのX成分の差を求める
	Vector3 screenPos = Transform({ 0.0f, 0.0f, 0.0f }, transform_->GetWorldMatrix() * pFollowCamera_->GetVpvpMatrix());
	Vector3 screenPosPrev = Transform({ 0.0f, 0.0f, 0.0f }, transform_->GetWorldMatrixPrev() * pFollowCamera_->GetVpvpMatrix());
	screenPos_ = Vector2(screenPos.x, screenPos.y);
	screenPosPrev_ = Vector2(screenPosPrev.x, screenPosPrev.y);

	// ターゲット角度の生計算
	float diffX = std::abs(screenPos_.x) - std::abs(screenPosPrev_.x);
	diffX *= -1.0f; // 向きを反転

	// Clamp
	diffX = std::clamp(diffX, -0.05f, 0.05f);

	// kは今回どれだけターゲットに寄せるかの係数
	// smoothSpeed_ が大きいほど反応が速い
	float k = 1.0f - std::exp(-5 * GameTimer::DeltaTime());
	k = std::clamp(k, 0.0f, 1.0f);

	smoothedDiffX_ += (diffX - smoothedDiffX_) * k;

	// もし安全のため最終値も範囲内に留めたいなら
	smoothedDiffX_ = std::clamp(smoothedDiffX_, -0.05f, 0.05f);

	pFollowCamera_->SetAngleZ(smoothedDiffX_);
}
