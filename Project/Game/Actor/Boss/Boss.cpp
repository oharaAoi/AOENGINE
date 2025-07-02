#include "Boss.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/System/Collision/ColliderCollector.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Scene/SceneLoader.h"
#include "Game/UI/Boss/BossUIs.h"
#include "Game/Information/ColliderCategory.h"
#include "Game/Actor/Boss/State/BossStateNormal.h"
#include "Game/Actor/Boss/State/BossStateStan.h"
#include "Game/Actor/Boss/Action/BossActionIdle.h"
#include "Game/Actor/Boss/Action/Move/BossActionApproach.h"
#include "Game/Actor/Boss/Action/Move/BossActionLeave.h"
#include "Game/Actor/Boss/Action/Move/BossActionStrafe.h"
#include "Game/Actor/Boss/Action/Move/BossActionKeepDistance.h"
#include "Game/Actor/Boss/Action/Attack/BossActionShotMissile.h"
#include "Game/Actor/Boss/Action/Attack/BossActionShotBullet.h"
#include "Game/Actor/Boss/Action/Attack/BossActionShotLauncher.h"
#include "Game/Actor/Boss/Action/Attack/BossActionShotgun.h"
#include "Game/Actor/Boss/Action/Attack/BossActionAllRangeMissile.h"
#include "Game/Actor/Boss/Action/BossActionDeployArmor.h"

void Boss::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Debug_Gui() {
	transform_->Debug_Gui();

	if (ImGui::CollapsingHeader("Parameter")) {
		ImGui::DragFloat("Health", &param_.health, 0.1f);
		ImGui::DragFloat("postureStability", &param_.postureStability, 0.1f);

		if (ImGui::Button("Reset")) {
			initParam_.FromJson(JsonItems::GetData(GetName(), param_.GetName()));
			param_ = initParam_;
		}
	}

	if (ImGui::CollapsingHeader("InitParameter")) {
		ImGui::DragFloat("Health", &initParam_.health, 0.1f);
		ImGui::DragFloat("postureStability", &initParam_.postureStability, 0.1f);

		if (ImGui::Button("Save")) {
			JsonItems::Save(GetName(), initParam_.ToJson(param_.GetName()));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Init() {
	SetName("Boss");
	SceneLoader::Objects object = SceneLoader::GetInstance()->GetObjects("Boss");

	object_ = SceneRenderer::GetInstance()->GetGameObject<BaseGameObject>("Boss");
	transform_ = object_->GetTransform();
	
	ICollider* collider = object_->GetCollider();
	collider->SetCategory(ColliderTags::Boss::own);
	collider->SetTarget(ColliderTags::Bullet::machinegun);
	collider->SetTarget(ColliderTags::Field::ground);
	collider->SetTarget(ColliderTags::None::own);
	collider->SetIsStatic(false);

	object_->SetPhysics();
	object_->GetRigidbody()->SetGravity(false);

	AI_ = std::make_unique<BossAI>();
	AI_->Init();
	this->AddChild(AI_.get());

	// -------------------------------------------------
	// ↓ Action関連
	// -------------------------------------------------

	actionManager_ = std::make_unique<ActionManager<Boss>>();
	actionManager_->Init(this, "BossAction");
	actionManager_->BuildAction<BossActionIdle>();
	actionManager_->BuildAction<BossActionApproach>();
	actionManager_->BuildAction<BossActionKeepDistance>();
	actionManager_->BuildAction<BossActionLeave>();
	actionManager_->BuildAction<BossActionStrafe>();

	actionManager_->BuildAction<BossActionShotMissile>();
	actionManager_->BuildAction<BossActionShotBullet>();
	actionManager_->BuildAction<BossActionShotLauncher>();
	actionManager_->BuildAction<BossActionShotgun>();
	actionManager_->BuildAction<BossActionAllRangeMissile>();
	actionManager_->BuildAction<BossActionDeployArmor>();

	size_t hash = typeid(BossActionDeployArmor).hash_code();
	actionManager_->AddRunAction(hash);

	// -------------------------------------------------
	// ↓ State関連
	// -------------------------------------------------

	stateMachine_ = std::make_unique<StateMachine<Boss>>();
	stateMachine_->Init(this);
	stateMachine_->ChangeState<BossStateNormal>();

	// -------------------------------------------------
	// ↓ weapon関連
	// -------------------------------------------------
	pulseArmor_ = SceneRenderer::GetInstance()->AddObject<PulseArmor>("BossPulseArmor", "Object_Dissolve.json", 100);
	pulseArmor_->Init();
	pulseArmor_->GetTransform()->SetParent(transform_->GetWorldMatrix());
	this->AddChild(pulseArmor_);

	// -------------------------------------------------
	// ↓ State関連
	// -------------------------------------------------

	initParam_.FromJson(JsonItems::GetData(GetName(), param_.GetName()));
	param_ = initParam_;

	param_.postureStability -= initParam_.postureStability;
	isAlive_ = false;
	isStan_ = false;

	EditorWindows::AddObjectWindow(this, "Boss");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Update() {
	if (!isAlive_) { return; }

	actionManager_->Update();
	stateMachine_->Update();

	pulseArmor_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Draw() const {
	pulseArmor_->Draw();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Damageを与える
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Damage(float _takeDamage) {
	// スタン状態にする
	if (!pulseArmor_->GetIsAlive()) {
		param_.health -= _takeDamage;

		if (!isStan_) {
			param_.postureStability += _takeDamage * 0.3f;
			param_.postureStability = std::clamp(param_.postureStability, 0.0f, initParam_.postureStability);
		}

		if (param_.postureStability >= initParam_.postureStability) {
			stateMachine_->ChangeState<BossStateStan>();
		}
	} else {
		pulseArmor_->DamageDurability(_takeDamage * 0.3f);
		if (pulseArmor_->BreakArmor()) {
			stateMachine_->ChangeState<BossStateStan>();
		}
	}

	// 倒した
	if (param_.health <= 0.0f) {
		isAlive_ = false;
	}
}

void Boss::ResetStan() {
	isStan_ = false;
	param_.postureStability -= initParam_.postureStability;
}
