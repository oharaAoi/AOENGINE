#include "Boss.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/System/Collision/ColliderCollector.h"
#include "Game/Information/ColliderCategory.h"
#include "Game/Actor/Boss/State/BossIdleState.h"

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
	ColliderCollector::AddCollider(collider_.get());

	// -------------------------------------------------
	// ↓ State関連
	// -------------------------------------------------
	stateMachine_ = std::make_unique<StateMachine<Boss>>();
	stateMachine_->Init(this);
	stateMachine_->ChangeState<BossIdleState>();

	transform_->translate_.z = 25.0f;
	transform_->rotation_ = Quaternion::AngleAxis(kPI, CVector3::UP);

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, "Boss");
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Update() {
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
}
#endif // _DEBUG