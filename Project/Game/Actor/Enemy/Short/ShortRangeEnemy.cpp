#include "ShortRangeEnemy.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/AI/BehaviorTreeSystem.h"
#include "Game/Information/ColliderCategory.h"
#include "Game/Actor/Enemy/Action/EnemyActionWait.h"
#include "Game/Actor/Enemy/Action/EnemyActionApproach.h"
#include "Game/Actor/Enemy/Action/EnemyActionAttack.h"
#include "Game/Actor/Enemy/Action/EnemyActionQuickDash.h"
#include "Game/Actor/Enemy/Action/EnemyActionStepBack.h"
#include "Game/Actor/Weapon/MachineGun.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShortRangeEnemy::Init() {
	SetName("ShortRangeEnemy");
	type_ = EnemyType::Long;
	baseParam_.SetGroupName(param_.GetGroupName());
	param_.Load();
	baseParam_.Load();

	// ----------------------
	// ↓ objectをシーンに追加する
	// ----------------------
	object_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<AOENGINE::BaseGameObject>("ShortRangeEnemy", "Object_PBR.json", 100);

	InitCommon();

	// ----------------------
	// ↓ Colliderの追加
	// ----------------------
	AOENGINE::BaseCollider* collider = object_->SetCollider(ColliderTags::Enemy::own, ColliderShape::Sphere);
	collider->SetIsStatic(false);
	collider->SetTarget(ColliderTags::Field::invisibleWall);
	collider->SetTarget(ColliderTags::Bullet::machinegun);
	collider->SetTarget(ColliderTags::Bullet::launcher);
	collider->SetTarget(ColliderTags::Bullet::rocket);
	collider->SetTarget(ColliderTags::Bullet::laser);
	collider->SetLocalPos(param_.colliderLocalPos);
	collider->SetRadius(param_.colliderRadius);

	searchCollider_->SetRadius(param_.searchRange);

	// ----------------------
	// ↓ 武器を生成
	// ----------------------
	weapon_ = std::make_unique<MachineGun>();
	weapon_->Init();
	weapon_->GetTransform()->SetParent(rightHandMatrix_);
	weapon_->GetTransform()->SetTranslate(param_.weaponOffset);
	weapon_->GetTransform()->SetRotate(Math::Quaternion::EulerToQuaternion(param_.weaponRotate));

	// ----------------------
	// ↓ AI関連の実装
	// ----------------------

	blackboard_ = std::make_unique<AI::Blackboard>();
	blackboard_->Load(param_.worldStatePath);
	blackboard_->SetRef<bool>("isTargetDiscovery", isTargetDiscovery_);

	behaviorTree_ = AI::BehaviorTreeSystem::GetInstance()->Create();
	behaviorTree_->Init();
	behaviorTree_->SetName("ShortRangeEnemyTree.aitree");
	behaviorTree_->SetBlackboard(blackboard_.get());
	behaviorTree_->SetTarget(this);
	behaviorTree_->Register("Wait", [this]() { return CreateTask<EnemyActionWait>(this, "Wait"); });
	behaviorTree_->Register("Approach", [this]() { return CreateTask<EnemyActionApproach>(this, "Approach"); });
	behaviorTree_->Register("Attack", [this]() { return CreateTask<EnemyActionAttack>(this, "Attack"); });
	behaviorTree_->Register("QuickDash", [this]() { return CreateTask<EnemyActionQuickDash>(this, "QuickDash"); });
	behaviorTree_->Register("StepBack", [this]() { return CreateTask<EnemyActionStepBack>(this, "StepBack"); });
	behaviorTree_->CreateTree("./Project/Packages/Game/GameData/BehaviorTree/", "ShortRangeEnemyTree.aitree");
	behaviorTree_->SetExecute(false);

	transform_->SetOffset(param_.translateOffset);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShortRangeEnemy::Update() {
	behaviorTree_->Run();

	if (pEnemyBulletManager_) {
		weapon_->SetBulletManager(pEnemyBulletManager_);
	}
	weapon_->Update();

	AOENGINE::Skeleton* skeleton = object_->GetAnimator()->GetSkeleton();
	rightHandMatrix_ = skeleton->GetSkeletonSpaceMat("right_hand") * transform_->GetWorldMatrix();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShortRangeEnemy::Debug_Gui() {
	param_.worldStatePath = blackboard_->GetPath();

	BaseEnemy::Debug_Gui();
	param_.Debug_Gui();

	AOENGINE::BaseCollider* collider = object_->GetCollider(ColliderTags::Enemy::own);
	collider->SetRadius(param_.colliderRadius);
	collider->SetLocalPos(param_.colliderLocalPos);
	searchCollider_->SetRadius(param_.searchRange);
	weapon_->GetTransform()->SetTranslate(param_.weaponOffset);
	weapon_->GetTransform()->SetRotate(Math::Quaternion::EulerToQuaternion(param_.weaponRotate));
	transform_->SetOffset(param_.translateOffset);
}

void ShortRangeEnemy::Parameter::Debug_Gui() {
	ImGui::DragFloat("索敵範囲", &searchRange, 0.1f);
	ImGui::DragFloat("コライダーの大きさ", &colliderRadius, 0.1f);
	ImGui::DragFloat3("コライダーのローカル座標", &colliderLocalPos.x, 0.1f);
	ImGui::DragFloat3("武器のオフセット", &weaponOffset.x, 0.1f);
	ImGui::DragFloat3("武器の回転", &weaponRotate.x, 0.1f);
	ImGui::DragFloat3("座標のオフセット", &translateOffset.x, 0.1f);
	ImGui::Text("text : %s", worldStatePath.c_str());
	SaveAndLoad();
}