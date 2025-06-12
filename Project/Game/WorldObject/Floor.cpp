#include "Floor.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/System/Collision/ColliderCollector.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Scene/SceneLoader.h"
#include "Game/Information/ColliderCategory.h"

Floor::Floor() {}
Floor::~Floor() { Finalize(); }

void Floor::Finalize() {
	BaseGameObject::Finalize();
}

void Floor::Init() {
	SetName("Ground");
	BaseGameObject::Init();

	SceneLoader::Objects object = SceneLoader::GetInstance()->GetObjects("Field");
	SetObject(object.modelName);
	transform_->SetSRT(object.srt);

	SetCollider(ColliderTags::Field::ground, ColliderShape::AABB);
	collider_->SetTarget(ColliderTags::Boss::own);
	collider_->SetTarget(ColliderTags::Boss::missile);
	collider_->SetTarget(ColliderTags::Player::own);
	collider_->SetSize(Vector3(300.f, 1.0f, 300.f));
	ColliderCollector::AddCollider(collider_.get());

	/*for (uint32_t oi = 0; oi < materials.size(); ++oi) {
		materials[oi]->SetUvScale(Vector3{50.0f, 50.0f, 1.0f});
	}*/

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
