#include "Boss.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/System/Collision/ColliderCollector.h"
#include "Engine/Render/SceneRenderer.h"
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
	BaseGameObject::Init();
	SetObject("boss.obj");

	SetCollider(ColliderTags::Boss::own, ColliderShape::SPHERE);
	collider_->SetTarget(ColliderTags::Bullet::machinegun);
	collider_->SetIsStatic(false);
	collider_->SetRadius(4.5f);

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

	transform_->translate_.z = 25.0f;
	transform_->translate_.y = 5.0f;
	transform_->rotation_ = Quaternion::AngleAxis(kPI, CVector3::UP);

	SceneRenderer::GetInstance()->SetObject(Object3dPSO::Normal, this);

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, "Boss");
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Update() {
	actionManager_->Update();
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