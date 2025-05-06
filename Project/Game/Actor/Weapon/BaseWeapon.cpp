#include "BaseWeapon.h"

void BaseWeapon::Finalize() {
}

void BaseWeapon::Init() {
	BaseGameObject::Init();
}

void BaseWeapon::Update() {
	BaseGameObject::Update();
}

void BaseWeapon::Draw() const {
	BaseGameObject::Draw();
}

#ifdef _DEBUG
void BaseWeapon::Debug_Gui() {
}
#endif // _DEBUG