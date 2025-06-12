#include "Boss.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
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
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Init() {
	SetName("Boss");
	BaseGameObject::Init();
	SceneLoader::Objects object = SceneLoader::GetInstance()->GetObjects("Boss");

	transform_->SetSRT(object.srt);
	SetObject(object.modelName);

	SetCollider(ColliderTags::Boss::own, object.colliderType);
	collider_->SetSize(object.colliderSize);
	collider_->SetLoacalPos(object.colliderCenter);
	collider_->SetTarget(ColliderTags::Bullet::machinegun);
	collider_->SetTarget(ColliderTags::Field::ground);
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

	SceneRenderer::GetInstance()->SetObject("Object_Normal.json", this);

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, "Boss");
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Update() {
	//actionManager_->Update();
	stateMachine_->Update();
	BaseGameObject::Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Draw() const {
	BaseGameObject::Draw();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void Boss::Debug_Gui() {
	transform_->Debug_Gui();
	collider_->Debug_Gui();
}
#endif // _DEBUG