#include "BaseWeapon.h"
#include "Engine/Render/SceneRenderer.h"

void BaseWeapon::Finalize() {
}

void BaseWeapon::Init() {
	object_ = SceneRenderer::GetInstance()->AddObject<BaseGameObject>("Weapon", "Object_Normal.json");
	transform_ = object_->GetTransform();
}

void BaseWeapon::Update() {
	
}

void BaseWeapon::Draw() const {
	
}

void BaseWeapon::Debug_Gui() {
	transform_->Debug_Gui();
	ImGui::DragFloat("Bullet Speed", &speed_, 0.1f);
}