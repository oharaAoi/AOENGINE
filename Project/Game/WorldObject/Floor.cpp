#include "Floor.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Game/Information/ColliderCategory.h"
#include "Engine/System/Collision/ColliderCollector.h"

Floor::Floor() {}
Floor::~Floor() {}

void Floor::Finalize() {
}

void Floor::Init() {
	BaseGameObject::Init();
	SetName("Ground");
	SetObject("floor.obj");
	SetIsLighting(false);

	SetCollider(ColliderTags::Field::ground, ColliderShape::AABB);
	collider_->SetTarget(ColliderTags::Boss::own);
	collider_->SetTarget(ColliderTags::Player::own);
	collider_->SetSize(Vector3(100.f, 1.f, 100.f));
	ColliderCollector::AddCollider(collider_.get());
	
	transform_->translate_.y = -0.1f;

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, GetName());
#endif // _DEBUG
}

void Floor::Update() {
	BaseGameObject::Update();
}

void Floor::Draw() const {
	BaseGameObject::Draw();
}
