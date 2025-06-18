#include "BaseWeapon.h"
#include "Engine/Render/SceneRenderer.h"

void BaseWeapon::Finalize() {
}

void BaseWeapon::Init() {
	weapon_ = SceneRenderer::GetInstance()->AddObject("Weapon", "Object_Normal.json");
	transform_ = weapon_->GetTransform();
}

void BaseWeapon::Update() {
	
}

void BaseWeapon::Draw() const {
	
}

#ifdef _DEBUG
void BaseWeapon::Debug_Gui() {
	transform_->Debug_Gui();
	ImGui::DragFloat("Bullet Speed", &speed_, 0.1f);
}
#endif // _DEBUG