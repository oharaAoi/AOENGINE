#include "Sword.h"
#include "Engine/Render/SceneRenderer.h"

void Sword::Init() {
	SetName("TestObject");
	object_ = SceneRenderer::GetInstance()->AddObject<BaseGameObject>("Sword", "Object_Normal.json");
	transform_ = object_->GetTransform();
	object_->SetObject("sword.obj");
	transform_->SetScale(CVector3::UNIT * 100.0f);
	
	EditorWindows::AddObjectWindow(this, "Sword");
}

void Sword::Update() {

}

void Sword::Debug_Gui() {
}
