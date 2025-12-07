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

	floor_ = AOENGINE::SceneRenderer::GetInstance()->GetGameObject<AOENGINE::BaseGameObject>("ground");
	AOENGINE::SceneRenderer::GetInstance()->SetRenderingQueue("ground", -100);
	transform_ = floor_->GetTransform();
	
	AOENGINE::SceneLoader::Objects object = AOENGINE::SceneLoader::GetInstance()->GetObjects("ground");
	floor_->SetEnableShadow(false);

	AOENGINE::EditorWindows::AddObjectWindow(this, GetName());
}

void Floor::Debug_Gui() {
}