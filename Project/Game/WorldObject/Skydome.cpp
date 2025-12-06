#include "Skydome.h"
#include "Engine/Render/SceneRenderer.h"

Skydome::Skydome() {}
Skydome::~Skydome() {}

void Skydome::Init() {
	object_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<AOENGINE::BaseGameObject>("skydome", "Object_Normal.json");
	transform_ = object_->GetTransform();
	object_->SetObject("skydome.obj");
}
