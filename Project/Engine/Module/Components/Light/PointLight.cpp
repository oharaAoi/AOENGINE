#include "PointLight.h"
#include "Engine/Lib/Json/JsonItems.h"


PointLight::PointLight() {
}

PointLight::~PointLight() {
}

void PointLight::Init(ID3D12Device* device, const size_t& size) {
	AttributeGui::SetName("Point Light");
	BaseLight::Init(device, size);
	lightBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));

	parameter_.SetGroupName("Light");
	pointLightData_->color = parameter_.color;
	pointLightData_->position = parameter_.position;
	pointLightData_->intensity = parameter_.intensity;
	pointLightData_->radius = parameter_.radius;
	pointLightData_->decay = parameter_.decay;
}

void PointLight::Finalize() {
	BaseLight::Finalize();
}

void PointLight::Update() {
	CalucViewProjection(lightPos_);
	pointLightData_->viewProjection = viewProjectionMatrix_;
	BaseLight::Update();
}

void PointLight::Draw(ID3D12GraphicsCommandList* commandList, const uint32_t& rootParameterIndex) {
	BaseLight::Draw(commandList, rootParameterIndex);
}

void PointLight::Debug_Gui() {
	pointLightData_->color = parameter_.color;
	pointLightData_->position = parameter_.position;
	pointLightData_->intensity = parameter_.intensity;
	pointLightData_->radius = parameter_.radius;
	pointLightData_->decay = parameter_.decay;
}

void PointLight::Paramter::Debug_Gui() {
	ImGui::ColorEdit4("color", &color.r);
	ImGui::DragFloat3("position", &position.x, 0.1f);
	ImGui::DragFloat("intensity", &intensity, 0.1f, 0.0f, 1.0f);
	ImGui::DragFloat("radius", &radius, 0.1f, 0.0f, 10.0f);
	ImGui::DragFloat("decay", &decay, 0.1f, 0.0f, 1.0f);

	SaveAndLoad();
}