#include "Skydome.h"
#include "Engine/Render/SceneRenderer.h"

Skydome::Skydome() {}
Skydome::~Skydome() {}

void Skydome::Init() {
	object_ = SceneRenderer::GetInstance()->AddObject<BaseGameObject>("skydome", "Object_Normal.json");
	transform_ = object_->GetTransform();
	object_->SetObject("skydome.obj");
}
