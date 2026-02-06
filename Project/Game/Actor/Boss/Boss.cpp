#include "Boss.h"
#include <algorithm>
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Scene/SceneLoader.h"
#include "Engine/System/AI/BehaviorTreeSystem.h"
#include "Engine/System/Input/Input.h"
#include "Game/UI/Boss/BossUIs.h"
#include "Game/Actor/Boss/State/BossStateNormal.h"
#include "Game/Actor/Boss/State/BossStateStun.h"
#include "Game/Actor/Boss/State/BossStateBeDestroyed.h"

#include "Game/Actor/Boss/Action/BossActionWait.h"
#include "Game/Actor/Boss/Action/Move/BossActionFloat.h"
#include "Game/Actor/Boss/Action/Move/BossActionApproach.h"
#include "Game/Actor/Boss/Action/Move/BossActionStrafe.h"
#include "Game/Actor/Boss/Action/Move/BossActionKeepDistance.h"
#include "Game/Actor/Boss/Action/Move/BossActionAdjustHeight.h"
#include "Game/Actor/Boss/Action/Move/BossActionStepBack.h"
#include "Game/Actor/Boss/Action/Attack/BossActionShotMissile.h"
#include "Game/Actor/Boss/Action/Attack/BossActionShotBullet.h"
#include "Game/Actor/Boss/Action/Attack/BossActionShotLauncher.h"
#include "Game/Actor/Boss/Action/Attack/BossActionShotgun.h"
#include "Game/Actor/Boss/Action/Attack/BossActionAllRangeMissile.h"
#include "Game/Actor/Boss/Action/Attack/BossActionVerticalMissile.h"
#include "Game/Actor/Boss/Action/Attack/BossActionRapidfire.h"
#include "Game/Actor/Boss/Action/Attack/BossActionDualStageMissile.h"
#include "Game/Actor/Boss/Action/BossActionDeployArmor.h"
#include "Game/Actor/Boss/Action/BossActionTransitionPhase.h"

#include "Game/Actor/Boss/GoalOriented/TargetDeadOriented.h"
#include "Game/Actor/Boss/GoalOriented/SafeDistanceOriented.h"

void Boss::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Debug_Gui() {
	if (object_) {
		object_->Debug_Gui();
	}

	if (ImGui::CollapsingHeader("現在のパラメータ")) {
		baseParam_.Debug_Gui();
		param_.Debug_Gui();
	}

	if (ImGui::CollapsingHeader("初期値のパラメータ(保存の際はこちらを編集)")) {
		initBaseParam_.Debug_Gui();
		initParam_.Debug_Gui();
	}

	if (ImGui::CollapsingHeader("積極性を計算するためのパラメータ")) {
		aggressionWeights_.Debug_Gui();
	}
	ImGui::Separator();
}

void Boss::Parameter::Debug_Gui() {
	ImGui::DragFloat("armorCoolTime", &armorCoolTime, 0.1f);
	ImGui::DragFloat("angularVelocity", &angularVelocity, 0.1f);
	ImGui::DragFloat("angularThreshold", &angularThreshold, 0.1f);
	ImGui::DragFloat("理想距離", &idealDistance, 0.1f);
	ImGui::DragFloat("最大距離", &maxDistance, 0.1f);
	ImGui::DragFloat("積極性のベース値", &aggressionBaseScore, 0.1f);
	ImGui::DragFloat2("ボスの状態を表示するwindowのoffset", &treeStateOffset.x, 0.1f);
	ImGui::Text("text : %s", worldStatePath.c_str());
	SaveAndLoad();
}

void Boss::AggressionWeights::Debug_Gui() {
	ImGui::DragFloat("基本", &base);
	ImGui::DragFloat("ヘルス", &health);
	ImGui::DragFloat("距離", &distance);
	SaveAndLoad();
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Init() {
	SetName("Boss");
	type_ = EnemyType::Boss;
	AOENGINE::SceneLoader::Objects object = AOENGINE::SceneLoader::GetInstance()->GetObjects("Boss");

	object_ = AOENGINE::SceneRenderer::GetInstance()->GetGameObject<AOENGINE::BaseGameObject>("Boss");
	transform_ = object_->GetTransform();
	
	AOENGINE::BaseCollider* collider = object_->GetCollider("boss");
	collider->SetIsStatic(false);

	object_->SetPhysics();
	object_->GetRigidbody()->SetGravity(false);

	initParam_.SetGroupName("Boss");
	initBaseParam_.SetGroupName("Boss");
	initParam_.Load();
	initBaseParam_.Load();
	param_ = initParam_;
	baseParam_ = initBaseParam_;

	aggressionWeights_.Load();

	// -------------------------------------------------
	// ↓ Tree関連
	// -------------------------------------------------

	blackboard_ = std::make_unique<AI::Blackboard>();
	blackboard_->Load(param_.worldStatePath);
	blackboard_->SetRef<float>("hp", baseParam_.health);
	blackboard_->SetRef<float>("maxHp", initBaseParam_.health);
	blackboard_->SetRef<float>("maxPostureStability", baseParam_.postureStability);
	blackboard_->SetRef<float>("aggressionScore", aggressionScore_);
	
	behaviorTree_ = AI::BehaviorTreeSystem::GetInstance()->Create();
	behaviorTree_->Init();
	behaviorTree_->SetName("BossTree.aitree");
	behaviorTree_->SetBlackboard(blackboard_.get());
	behaviorTree_->SetTarget(this);
	behaviorTree_->AddGoal(std::make_shared<TargetDeadOriented>());
	behaviorTree_->AddGoal(std::make_shared<SafeDistanceOriented>());
	behaviorTree_->Register("Wait", [this]() { return CreateTask<BossActionWait>(this, "Wait"); });
	behaviorTree_->Register("Float", [this]() { return CreateTask<BossActionFloat>(this, "Float"); });
	behaviorTree_->Register("KeepDistance", [this]() { return CreateTask<BossActionKeepDistance>(this, "KeepDistance"); });
	behaviorTree_->Register("Strafe", [this]() { return CreateTask<BossActionStrafe>(this, "Strafe"); });
	behaviorTree_->Register("StepBack", [this]() { return CreateTask<BossActionStepBack>(this, "StepBack"); });
	behaviorTree_->Register("ShotMissile", [this]() { return CreateTask<BossActionShotMissile>(this, "ShotMissile"); });
	behaviorTree_->Register("ShotBullet", [this]() { return CreateTask<BossActionShotBullet>(this, "ShotBullet"); });
	behaviorTree_->Register("Approach", [this]() { return CreateTask<BossActionApproach>(this, "Approach"); });
	behaviorTree_->Register("ShotLauncher", [this]() { return CreateTask<BossActionShotLauncher>(this, "ShotLauncher"); });
	behaviorTree_->Register("Shotgun", [this]() { return CreateTask<BossActionShotgun>(this, "Shotgun"); });
	behaviorTree_->Register("AllRangeMissile", [this]() { return CreateTask<BossActionAllRangeMissile>(this, "AllRangeMissile"); });
	behaviorTree_->Register("VerticalMissile", [this]() { return CreateTask<BossActionVerticalMissile>(this, "VerticalMissile"); });
	behaviorTree_->Register("DeployArmor", [this]() { return CreateTask<BossActionDeployArmor>(this, "DeployArmor"); });
	behaviorTree_->Register("Rapidfire", [this]() { return CreateTask<BossActionRapidfire>(this, "Rapidfire"); });
	behaviorTree_->Register("AdjustHeight", [this]() { return CreateTask<BossActionAdjustHeight>(this, "AdjustHeight"); });
	behaviorTree_->Register("DualStageMissile", [this]() { return CreateTask<BossActionDualStageMissile>(this, "DualStageMissile"); });
	behaviorTree_->Register("TransitionPhase", [this]() { return CreateTask<BossActionTransitionPhase>(this, "TransitionPhase"); });
	behaviorTree_->CreateTree("./Project/Packages/Game/Assets/GameData/BehaviorTree/", "BossTree.aitree");
	behaviorTree_->SetExecute(false);

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

	baseParam_.postureStability -= initBaseParam_.postureStability;
	isAlive_ = true;
	isStun_ = false;
	isDeployingArmor_ = false;
	isAttack_ = false;
	isMove_ = false;

	phase_ = BossPhase::First;

	actionStrategy_ = ActionStrategy::Defensive;
	aggressionScore_ = 0.5f;

	AOENGINE::EditorWindows::AddObjectWindow(this, "Boss");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Update() {
	
	// ----------------------
	// ↓ 目標敵の座標を探索
	// ----------------------
	if (pTargetTransform_) {
		targetPos_ = pTargetTransform_->GetOffsetPos();
	}

	// ----------------------
	// ↓ blackboardの更新
	// ----------------------
	initParam_.worldStatePath = blackboard_->GetPath();
	blackboard_->Set<bool>("isDeployingArmor", isDeployingArmor_);
	blackboard_->Set<bool>("isArmorDeployed", pulseArmor_->BreakArmor());
	blackboard_->Set<bool>("isAttack", isAttack_);
	blackboard_->Set<int32_t>("bossPhase", (int32_t)phase_);
	blackboard_->Set<float>("halfHp", initBaseParam_.health * 0.5f);
	float bossToTargetDistanceY = std::abs(targetPos_.y - GetPosition().y);
	blackboard_->Set<float>("bossToTargetDistanceY", bossToTargetDistanceY);
	if (pTargetTransform_) {
		blackboard_->Set<float>("BossToPlayer", (transform_->GetPos() - pTargetTransform_->GetPos()).Length());
	}

	// ----------------------
	// ↓ stateの更新
	// ----------------------
	if (!isAlive_) {
		stateMachine_->Update();
		return;
	}

	// ----------------------
	// ↓ treeの更新
	// ----------------------

	CalcAggression();

	behaviorTree_->Run();
	pulseArmor_->Update();
	stateMachine_->Update();

	// ----------------------
	// ↓ アーマーの状態を更新
	// ----------------------
	if (!pulseArmor_->GetIsAlive()) {
		param_.armorCoolTime -= AOENGINE::GameTimer::DeltaTime();
		if (behaviorTree_->GetRootNode()->GetState() == BehaviorStatus::Success
			|| behaviorTree_->GetRootNode()->GetState() == BehaviorStatus::Inactive) {
			if (param_.armorCoolTime <= 0.0f) {
				isDeployingArmor_ = true;
				param_.armorCoolTime = initParam_.armorCoolTime;
			}
		}
	}

#ifdef _DEBUG
	// treeの実行開始
	if (AOENGINE::Input::IsTriggerKey(DIK_M)) {
		behaviorTree_->SetExecute(true);
	}

	ImVec2 bossUIPos;
	if (Engine::WorldToGameImagePos(GetPosition(), bossUIPos)) {
		behaviorTree_->DisplayState(bossUIPos, aggressionScore_, ImVec2(param_.treeStateOffset.x, param_.treeStateOffset.y));
	}
	
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Damageを与える
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Damage(float _takeDamage) {
	// スタン状態にする
	if (!pulseArmor_->GetIsAlive()) {
		baseParam_.health -= _takeDamage;

		if (!isStun_) {
			baseParam_.postureStability += _takeDamage * baseParam_.postureStabilityScrapeRaito;
			baseParam_.postureStability = std::clamp(baseParam_.postureStability, 0.0f, initBaseParam_.postureStability);

			if (baseParam_.postureStability >= initBaseParam_.postureStability) {
				isStun_ = true;
				stateMachine_->ChangeState<BossStateStun>();
			}
		}

	} else {
		pulseArmor_->DamageDurability(_takeDamage * baseParam_.postureStabilityScrapeRaito);
		if (pulseArmor_->BreakArmor()) {
			stateMachine_->ChangeState<BossStateStun>();
		}
	}

	// 倒した
	if (isAlive_) {
		if (baseParam_.health <= 0.0f) {
			isAlive_ = false;
			stateMachine_->ChangeState<BossStateBeDestroyed>();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ stan解除
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::ResetStun() {
	isStun_ = false;
	baseParam_.postureStability -= initBaseParam_.postureStability;
	behaviorTree_->SetExecute(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Targetの方向を向く
///////////////////////////////////////////////////////////////////////////////////////////////

bool Boss::TargetLook() {
	// 目標の方向を計算する
	Math::Quaternion targetRotate = Math::Quaternion::LookRotation(pTargetTransform_->GetPos() - transform_->GetPos());
	// なす角を求める
	float angle = Math::Quaternion::Angle(transform_->GetRotate(), targetRotate);

	if (angle > param_.angularThreshold) {
		// 経過時間で変化させる角度
		float deltaAngle = param_.angularVelocity * AOENGINE::GameTimer::DeltaTime();
		// 実際に方向を決定
		transform_->SetRotate(Math::Quaternion::Slerp(transform_->GetRotate(), targetRotate, deltaAngle));
	} else {
		return true;
	}

	return false;
}

void Boss::CalcAggression() {
	// ----------------------
	// ↓ hpの計算
	// ----------------------
	float hpRaito = baseParam_.health / initBaseParam_.health;
	float heAggression = (1.0f - hpRaito) * aggressionWeights_.health;

	// ----------------------
	// ↓ 距離の計算
	// ----------------------
	float distance = (targetPos_ - GetPosition()).Length();
	float distanceScore = (distance - param_.idealDistance) / param_.maxDistance;
	float distanceAggression = std::clamp(distanceScore, 0.0f, 1.0f);

	// ----------------------
	// ↓ 最終スコアの計算
	// ----------------------
	aggressionScore_ = param_.aggressionBaseScore * aggressionWeights_.base;
	aggressionScore_ += heAggression * aggressionWeights_.health;
	aggressionScore_ += distanceAggression * aggressionWeights_.distance;

	aggressionScore_ = std::clamp(aggressionScore_, 0.0f, 1.0f);
}