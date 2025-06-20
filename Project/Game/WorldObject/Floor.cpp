#include "Floor.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/System/Collision/ColliderCollector.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Scene/SceneLoader.h"
#include "Game/Information/ColliderCategory.h"

Floor::Floor() {}
Floor::~Floor() { Finalize(); }

void Floor::Finalize() {
}

void Floor::Init() {
	SetName("Ground");

	floor_ = SceneRenderer::GetInstance()->GetGameObject("Field");
	transform_ = floor_->GetTransform();
	
	SceneLoader::Objects object = SceneLoader::GetInstance()->GetObjects("Field");
	
	floor_->SetCollider(ColliderTags::Field::ground, ColliderShape::AABB);
	ICollider* collider = floor_->GetCollider();
	collider->SetTarget(ColliderTags::Boss::own);
	collider->SetTarget(ColliderTags::Boss::missile);
	collider->SetTarget(ColliderTags::Player::own);
	collider->SetSize(Vector3(300.f, 1.0f, 300.f));
	
	/*for (uint32_t oi = 0; oi < materials.size(); ++oi) {
		materials[oi]->SetUvScale(Vector3{50.0f, 50.0f, 1.0f});
	}*/

	floor_->SetIsReflection(true);
	floor_->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));

	SceneRenderer::GetInstance()->ChangeRenderingType("Object_NormalEnviroment.json", floor_);

#ifdef _DEBUG
	EditorWindows::AddObjectWindow(this, GetName());
#endif // _DEBUG
}

void Floor::Update() {
	
}

void Floor::Draw() const {
	Engine::SetPipeline(PSOType::Object3d, "Object_NormalEnviroment.json");
}

#ifdef _DEBUG
void Floor::Debug_Gui() {
}
#endif // _DEBUG