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
