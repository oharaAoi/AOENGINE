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

	floor_ = SceneRenderer::GetInstance()->GetGameObject<BaseGameObject>("Field");
	transform_ = floor_->GetTransform();
	
	SceneLoader::Objects object = SceneLoader::GetInstance()->GetObjects("Field");
	
	ICollider* collider = floor_->GetCollider();
	collider->SetCategory(ColliderTags::None::own);
	collider->SetTarget(ColliderTags::Boss::own);
	collider->SetTarget(ColliderTags::Boss::missile);
	collider->SetTarget(ColliderTags::Player::own);
	
	/*for (uint32_t oi = 0; oi < materials.size(); ++oi) {
		materials[oi]->SetUvScale(Vector3{50.0f, 50.0f, 1.0f});
	}*/

	floor_->SetIsReflection(true);
	floor_->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));

	SceneRenderer::GetInstance()->ChangeRenderingType("Object_NormalEnviroment.json", floor_);

	EditorWindows::AddObjectWindow(this, GetName());
}

void Floor::Update() {
	
}

void Floor::Draw() const {
	Engine::SetPipeline(PSOType::Object3d, "Object_NormalEnviroment.json");
}

void Floor::Debug_Gui() {
}