#include "Floor.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/System/Collision/ColliderCollector.h"
#include "Engine/Render/SceneRenderer.h"
#include "Game/Information/ColliderCategory.h"

Floor::Floor() {}
Floor::~Floor() { Finalize(); }

void Floor::Finalize() {
	BaseGameObject::Finalize();
}

void Floor::Init() {
	BaseGameObject::Init();
	SetName("Ground");
	SetObject("floor.obj");
	SetIsLighting(false);

	SetCollider(ColliderTags::Field::ground, ColliderShape::AABB);
	collider_->SetTarget(ColliderTags::Boss::own);
	collider_->SetTarget(ColliderTags::Boss::missile);
	collider_->SetTarget(ColliderTags::Player::own);
	collider_->SetSize(Vector3(1000.f, 1.f, 1000.f));
	ColliderCollector::AddCollider(collider_.get());
	
	transform_->translate_.y = -0.1f;

	isReflection_ = true;

	SceneRenderer::GetInstance()->SetObject("Object_NormalEnviroment.json", this);

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, GetName());
#endif // _DEBUG
}

void Floor::Update() {
	BaseGameObject::Update();
}

void Floor::Draw() const {
	Engine::SetPipeline(PSOType::Object3d, "Object_NormalEnviroment.json");
	BaseGameObject::Draw();
}
