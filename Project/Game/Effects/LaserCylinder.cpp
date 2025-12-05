#include "LaserCylinder.h"

LaserCylinder::~LaserCylinder() {
	SceneRenderer::GetInstance()->ReleaseObject(object_);
}

void LaserCylinder::Init(const Color& _color) {
	object_ = SceneRenderer::GetInstance()->AddObject<BaseGameObject>("LaserCylinder", "Object_laser.json");
	object_->SetObject("laserCylinder.obj");
	object_->SetTexture("laser.png");
	object_->SetIsLighting(false);
	object_->SetEnableShadow(false);
	object_->SetColor(_color);
	transform_ = object_->GetTransform();
	transform_->SetTranslationZ(1.0f);

	shaderGraph_ = std::make_unique<ShaderGraph>();
	shaderGraph_->Init("laser.json");

}

void LaserCylinder::Update() {
	shaderGraph_->Update();
}

void LaserCylinder::Debug_Gui() {
	object_->Debug_Gui();
	shaderGraph_->Debug_Gui();
}

void LaserCylinder::SetUvScale(float _scaleZ) {
	for (auto& material : object_->GetMaterials()) {
		material.second->SetShaderGraph(shaderGraph_.get());
		material.second->SetUvScale(Vector3(1, _scaleZ, 1));
	}
}

void LaserCylinder::SetScaleZ(float _scaleZ) {
	transform_->SetScaleZ(_scaleZ);
}