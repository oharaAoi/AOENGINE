#include "Boss.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Scene/SceneLoader.h"
#include "Game/UI/Boss/BossUIs.h"
#include "Game/Actor/Boss/State/BossStateNormal.h"
#include "Game/Actor/Boss/State/BossStateStan.h"
#include "Game/Actor/Boss/State/BossStateBeDestroyed.h"

#include "Game/Actor/Boss/Action/BossActionWait.h"
#include "Game/Actor/Boss/Action/Move/BossActionFloat.h"
#include "Game/Actor/Boss/Action/Move/BossActionApproach.h"
#include "Game/Actor/Boss/Action/Move/BossActionLeave.h"
#include "Game/Actor/Boss/Action/Move/BossActionStrafe.h"
#include "Game/Actor/Boss/Action/Move/BossActionKeepDistance.h"
#include "Game/Actor/Boss/Action/Move/BossActionAdjustHeight.h"
#include "Game/Actor/Boss/Action/Attack/BossActionShotMissile.h"
#include "Game/Actor/Boss/Action/Attack/BossActionShotBullet.h"
#include "Game/Actor/Boss/Action/Attack/BossActionShotLauncher.h"
#include "Game/Actor/Boss/Action/Attack/BossActionShotgun.h"
#include "Game/Actor/Boss/Action/Attack/BossActionAllRangeMissile.h"
#include "Game/Actor/Boss/Action/Attack/BossActionVerticalMissile.h"
#include "Game/Actor/Boss/Action/Attack/BossActionRapidfire.h"
#include "Game/Actor/Boss/Action/BossActionDeployArmor.h"

#include "Game/Actor/Boss/GoalOriented/TargetDeadOriented.h"
#include "Game/Actor/Boss/GoalOriented/SafeDistanceOriented.h"
#include "Game/Actor/Boss/GoalOriented/DeployArmorOriented.h"

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
		ImGui::DragFloat("armorCoolTime", &param_.armorCoolTime, 0.1f);

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

	ImGui::Separator();
	ImGui::BulletText("BehaviorTree");
	behaviorTree_->Edit();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Init() {
	SetName("Boss");
	SceneLoader::Objects object = SceneLoader::GetInstance()->GetObjects("Boss");

	object_ = SceneRenderer::GetInstance()->GetGameObject<BaseGameObject>("Boss");
	transform_ = object_->GetTransform();
	
	ICollider* collider = object_->GetCollider("boss");
	collider->SetIsStatic(false);

	object_->SetPhysics();
	object_->GetRigidbody()->SetGravity(false);

	initParam_.FromJson(JsonItems::GetData(GetName(), param_.GetName()));
	param_ = initParam_;

	// -------------------------------------------------
	// ↓ Tree関連
	// -------------------------------------------------

	worldState_ = std::make_unique<BossWorldState>();
	worldState_->Set<float>("hp", param_.health);
	worldState_->Set<float>("maxHp", param_.health);
	worldState_->Set<float>("postureStability", 0);
	worldState_->Set<float>("maxPostureStability", param_.postureStability);
	worldState_->Set<float>("targetToDistance", 0.0f);
	worldState_->Set<float>("idealDistance", 50.0f);
	worldState_->Set<bool>("isTargetDead", false);
	worldState_->Set<bool>("isDeployArmor", true);

	behaviorTree_ = std::make_unique<BehaviorTree>();
	behaviorTree_->Init();
	behaviorTree_->SetName("Boss Behavior Tree");
	behaviorTree_->SetWorldState(worldState_.get());
	behaviorTree_->AddGoal(std::make_shared<TargetDeadOriented>());
	behaviorTree_->AddGoal(std::make_shared<SafeDistanceOriented>());
	behaviorTree_->AddGoal(std::make_shared<DeployArmorOriented>());
	behaviorTree_->AddCanTask(CreateTask<BossActionWait>(this, "Wait"));
	behaviorTree_->AddCanTask(CreateTask<BossActionFloat>(this, "Float"));
	behaviorTree_->AddCanTask(CreateTask<BossActionApproach>(this, "Approach"));
	behaviorTree_->AddCanTask(CreateTask<BossActionKeepDistance>(this, "KeepDistance"));
	behaviorTree_->AddCanTask(CreateTask<BossActionLeave>(this, "Leave"));
	behaviorTree_->AddCanTask(CreateTask<BossActionStrafe>(this, "Strafe"));
	behaviorTree_->AddCanTask(CreateTask<BossActionShotMissile>(this, "ShotMissile"));
	behaviorTree_->AddCanTask(CreateTask<BossActionShotBullet>(this, "ShotBullet"));
	behaviorTree_->AddCanTask(CreateTask<BossActionShotLauncher>(this, "ShotLauncher"));
	behaviorTree_->AddCanTask(CreateTask<BossActionShotgun>(this, "Shotgun"));
	behaviorTree_->AddCanTask(CreateTask<BossActionAllRangeMissile>(this, "AllRangeMissile"));
	behaviorTree_->AddCanTask(CreateTask<BossActionVerticalMissile>(this, "VerticalMissile"));
	behaviorTree_->AddCanTask(CreateTask<BossActionDeployArmor>(this, "DeployArmor"));
	behaviorTree_->AddCanTask(CreateTask<BossActionRapidfire>(this, "Rapidfire"));
	behaviorTree_->AddCanTask(CreateTask<BossActionAdjustHeight>(this, "AdjustHeight"));
	behaviorTree_->CreateTree("./Game/Assets/GameData/BehaviorTree/BossTree.json");
	behaviorTree_->SetExecute(true);

	evaluationFormula_ = std::make_unique<BossEvaluationFormula>();
	evaluationFormula_->Init(this);

	// -------------------------------------------------
	// ↓ State関連
	// -------------------------------------------------

	stateMachine_ = std::make_unique<StateMachine<Boss>>();
	stateMachine_->Init(this);
	stateMachine_->ChangeState<BossStateNormal>();

	// -------------------------------------------------
	// ↓ weapon関連
	// -------------------------------------------------
	pulseArmor_ = std::make_unique<Armors>();
	pulseArmor_->Init("Boss");
	pulseArmor_->SetParent(transform_->GetWorldMatrix());
	this->AddChild(pulseArmor_.get());

	// -------------------------------------------------
	// ↓ State関連
	// -------------------------------------------------

	param_.postureStability -= initParam_.postureStability;
	isAlive_ = true;
	isStan_ = false;
	isArmorDeploy_ = true;

	phase_ = BossPhase::FIRST;

	EditorWindows::AddObjectWindow(this, "Boss");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Update() {
	if (!isAlive_) {
		stateMachine_->Update();
		return;
	}

	if (!pulseArmor_->GetIsAlive()) {
		isArmorDeploy_ = false;
		param_.armorCoolTime -= GameTimer::DeltaTime();
		if (param_.armorCoolTime <= 0.0f) {
			isArmorDeploy_ = true;
			param_.armorCoolTime = initParam_.armorCoolTime;
		}
	}

	worldState_->Set("hp", param_.health);
	worldState_->Set("postureStability", param_.postureStability);
	worldState_->Set("targetToDistance", Length(GetPosition() - playerPosition_));
	worldState_->Set("isTargetDead", isTargetDead_);
	worldState_->Set("isDeployArmor", isArmorDeploy_);

	stateMachine_->Update();
	behaviorTree_->Run();
	evaluationFormula_->Update(); 
	pulseArmor_->Update();

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
		stateMachine_->ChangeState<BossStateBeDestroyed>();
	}
}

void Boss::ResetStan() {
	isStan_ = false;
	param_.postureStability -= initParam_.postureStability;
	behaviorTree_->SetExecute(true);
}
