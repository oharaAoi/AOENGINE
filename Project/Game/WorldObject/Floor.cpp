#include "Floor.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/System/Collision/ColliderCollector.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Scene/SceneLoader.h"
#include "Game/Information/ColliderCategory.h"

Floor::Floor() {}
Floor::~Floor() {}

void Floor::Init() {
	SetName("Ground");

	floor_ = SceneRenderer::GetInstance()->GetGameObject<BaseGameObject>("ground");
	SceneRenderer::GetInstance()->SetRenderingQueue("ground", -100);
	transform_ = floor_->GetTransform();
	
	SceneLoader::Objects object = SceneLoader::GetInstance()->GetObjects("ground");
	floor_->SetIsShadow(false);

	EditorWindows::AddObjectWindow(this, GetName());
}

void Floor::Debug_Gui() {
}