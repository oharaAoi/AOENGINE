#include "Skydome.h"

Skydome::Skydome() {}
Skydome::~Skydome() {}

void Skydome::Finalize() {
	BaseGameObject::Finalize();
}

void Skydome::Init() {
	BaseGameObject::Init();
	SetObject("skydome.obj");
	SetIsLighting(false);
}

void Skydome::Update() {
	BaseGameObject::Update();
}

void Skydome::Draw() const {
	Engine::SetPSOObj(Object3dPSO::Normal);
	BaseGameObject::Draw();
}
