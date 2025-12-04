#include "Boss.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Scene/SceneLoader.h"
#include "Engine/System/AI/BehaviorTreeSystem.h"
#include "Engine/System/Audio/AudioPlayer.h"
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
#include "Game/Actor/Boss/Action/Attack/BossActionDualStageMissile.h"
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
		param_.Debug_Gui();
	}

	if (ImGui::CollapsingHeader("InitParameter(保存の際はこちらを編集)")) {
		initParam_.Debug_Gui();
	}

	ImGui::Separator();
}

void Boss::Parameter::Debug_Gui() {
	ImGui::DragFloat("Health", &health, 0.1f);
	ImGui::DragFloat("postureStability", &postureStability, 0.1f);
	ImGui::DragFloat("armorCoolTime", &armorCoolTime, 0.1f);
	ImGui::DragFloat("angularVelocity", &angularVelocity, 0.1f);
	ImGui::DragFloat("angularThreshold", &angularThreshold, 0.1f);
	SaveAndLoad();
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

	initParam_.SetGroupName("Boss");
	initParam_.Load();
	param_ = initParam_;

	// -------------------------------------------------
	// ↓ Tree関連
	// -------------------------------------------------

	worldState_ = std::make_unique<BossWorldState>();
	worldState_->Load(param_.worldStatePath);
	worldState_->SetRef<float>("hp", param_.health);
	worldState_->SetRef<float>("maxHp", param_.health);
	worldState_->SetRef<float>("maxPostureStability", param_.postureStability);

	behaviorTree_ = BehaviorTreeSystem::GetInstance()->Create();
	behaviorTree_->Init();
	behaviorTree_->SetName("BossBehaviorTree");
	behaviorTree_->SetWorldState(worldState_.get());
	behaviorTree_->SetTarget(this);
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
	behaviorTree_->AddCanTask(CreateTask<BossActionDualStageMissile>(this, "DualStageMissile"));
	behaviorTree_->CreateTree("./Project/Packages/Game/Assets/GameData/BehaviorTree/BossTree.json");
	behaviorTree_->SetExecute(false);

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
	pulseArmor_->SetArmor();
	this->AddChild(pulseArmor_.get());

	// -------------------------------------------------
	// ↓ State関連
	// -------------------------------------------------

	param_.postureStability -= initParam_.postureStability;
	isAlive_ = true;
	isStan_ = false;
	isArmorDeploy_ = false;
	isAttack_ = false;
	isMove_ = false;

	phase_ = BossPhase::First;

	EditorWindows::AddObjectWindow(this, "Boss");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Update() {
	targetPos_ = targetTransform_->GetPos();

	initParam_.worldStatePath = worldState_->GetPath();
	worldState_->Set<float>("BossToPlayer", (transform_->GetPos() - targetTransform_->GetPos()).Length());
	worldState_->Set<bool>("deployArmor", isArmorDeploy_);
	worldState_->Set<bool>("isAttack", isAttack_);

	// stateの更新
	if (!isAlive_) {
		stateMachine_->Update();
		return;
	}

	// アーマーのクールタイムを更新
	if (!pulseArmor_->GetIsAlive()) {
		param_.armorCoolTime -= GameTimer::DeltaTime();
		if (param_.armorCoolTime <= 0.0f) {
			isArmorDeploy_ = true;
			param_.armorCoolTime = initParam_.armorCoolTime;
		}
	}

	// treeの実行開始
	if (Input::IsTriggerKey(DIK_M)) {
		behaviorTree_->SetExecute(true);
	}

	// 各項目の更新
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
			param_.postureStability += _takeDamage * param_.postureStabilityScrapeRaito;
			param_.postureStability = std::clamp(param_.postureStability, 0.0f, initParam_.postureStability);

			if (param_.postureStability >= initParam_.postureStability) {
				isStan_ = true;
				stateMachine_->ChangeState<BossStateStan>();
			}
		}

	} else {
		pulseArmor_->DamageDurability(_takeDamage * param_.postureStabilityScrapeRaito);
		if (pulseArmor_->BreakArmor()) {
			stateMachine_->ChangeState<BossStateStan>();
		}
	}

	// 倒した
	if (isAlive_) {
		if (param_.health <= 0.0f) {
			isAlive_ = false;
			stateMachine_->ChangeState<BossStateBeDestroyed>();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ stan解除
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::ResetStan() {
	isStan_ = false;
	param_.postureStability -= initParam_.postureStability;
	behaviorTree_->SetExecute(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Targetの方向を向く
///////////////////////////////////////////////////////////////////////////////////////////////

bool Boss::TargetLook() {
	// 目標の方向を計算する
	Quaternion targetRotate = Quaternion::LookRotation(targetTransform_->GetPos() - transform_->GetPos());
	// なす角を求める
	float angle = Quaternion::Angle(transform_->GetRotate(), targetRotate);

	if (angle > param_.angularThreshold) {
		// 経過時間で変化させる角度
		float deltaAngle = param_.angularVelocity * GameTimer::DeltaTime();
		// 実際に方向を決定
		transform_->SetRotate(Quaternion::Slerp(transform_->GetRotate(), targetRotate, deltaAngle));
	} else {
		return true;
	}

	return false;
}
