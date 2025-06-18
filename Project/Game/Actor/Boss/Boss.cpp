#include "Boss.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/System/Collision/ColliderCollector.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Scene/SceneLoader.h"
#include "Game/Information/ColliderCategory.h"
#include "Game/Actor/Boss/State/BossIdleState.h"
#include "Game/Actor/Boss/Action/BossActionIdle.h"
#include "Game/Actor/Boss/Action/BossActionApproach.h"
#include "Game/Actor/Boss/Action/BossActionShotMissile.h"
#include "Game/Actor/Boss/Action/BossActionAllRangeMissile.h"

void Boss::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
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
#endif // _DEBUG

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Init() {
	SetName("Boss");
	SceneLoader::Objects object = SceneLoader::GetInstance()->GetObjects("Boss");

	boss_ = SceneRenderer::GetInstance()->GetGameObject("Boss");
	transform_ = boss_->GetTransform();

	boss_->SetCollider(ColliderTags::Boss::own, object.colliderType);
	ICollider* collider = boss_->GetCollider();
	collider->SetSize(object.colliderSize);
	collider->SetLoacalPos(object.colliderCenter);
	collider->SetTarget(ColliderTags::Bullet::machinegun);
	collider->SetTarget(ColliderTags::Field::ground);
	//collider_->SetIsStatic(false);

	// -------------------------------------------------
	// ↓ Action関連
	// -------------------------------------------------

	actionManager_ = std::make_unique<ActionManager<Boss>>();
	actionManager_->Init(this, "bossAction");
	actionManager_->BuildAction<BossActionIdle>();
	actionManager_->BuildAction<BossActionApproach>();
	actionManager_->BuildAction<BossActionShotMissile>();
	actionManager_->BuildAction<BossActionAllRangeMissile>();

	size_t hash = typeid(BossActionIdle).hash_code();
	actionManager_->AddRunAction(hash);

	// -------------------------------------------------
	// ↓ State関連
	// -------------------------------------------------

	stateMachine_ = std::make_unique<StateMachine<Boss>>();
	stateMachine_->Init(this);
	stateMachine_->ChangeState<BossIdleState>();

	// -------------------------------------------------
	// ↓ State関連
	// -------------------------------------------------

	initParam_.FromJson(JsonItems::GetData(GetName(), param_.GetName()));
	param_ = initParam_;

	param_.postureStability -= initParam_.postureStability;

#ifdef _DEBUG
	EditorWindows::AddObjectWindow(this, "Boss");
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Update() {
	actionManager_->Update();
	stateMachine_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Draw() const {
	
}