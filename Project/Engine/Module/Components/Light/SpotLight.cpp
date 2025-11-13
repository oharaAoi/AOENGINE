#include "SpotLight.h"
#include "Engine/Lib/Json//JsonItems.h"

SpotLight::SpotLight() {
}

SpotLight::~SpotLight() {
}

void SpotLight::Finalize() {
	BaseLight::Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SpotLight::Init(ID3D12Device* device, const size_t& size) {
	AttributeGui::SetName("Spot Light");
	BaseLight::Init(device, size);
	lightBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));

	parameter_.SetGroupName("Light");
	spotLightData_->color = parameter_.color;
	spotLightData_->position = parameter_.position;
	spotLightData_->intensity = parameter_.intensity;
	spotLightData_->direction = parameter_.direction;
	spotLightData_->distance = parameter_.distance;
	spotLightData_->decay = parameter_.decay;
	spotLightData_->cosAngle = parameter_.cosAngle;
	spotLightData_->cosFalloffStart = parameter_.cosFalloffStart;

	cosDegree_ = std::cos(std::numbers::pi_v<float> / 3.0f);
	falloffDegree_ = std::cos(std::numbers::pi_v<float> / 4.0f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SpotLight::Update() {
	CalucViewProjection(lightPos_);
	spotLightData_->viewProjection = viewProjectionMatrix_;
	BaseLight::Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void SpotLight::BindCommand(ID3D12GraphicsCommandList* commandList, const uint32_t& rootParameterIndex) {
	BaseLight::BindCommand(commandList, rootParameterIndex);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SpotLight::Debug_Gui() {
	parameter_.Debug_Gui();
	ImGui::SliderFloat("cosAngle", &cosDegree_, 0.0f, (std::numbers::pi_v<float>));
	ImGui::SliderFloat("falloffDegree ", &falloffDegree_, 0.0f, (std::numbers::pi_v<float>));


	parameter_.direction = Normalize(parameter_.direction);
	parameter_.cosAngle = std::cos(cosDegree_);
	parameter_.cosFalloffStart = std::cos(falloffDegree_);

	spotLightData_->color = parameter_.color;
	spotLightData_->position = parameter_.position;
	spotLightData_->intensity = parameter_.intensity;
	spotLightData_->direction = parameter_.direction;
	spotLightData_->distance = parameter_.distance;
	spotLightData_->decay = parameter_.decay;
	spotLightData_->cosAngle = parameter_.cosAngle;
	spotLightData_->cosFalloffStart = parameter_.cosFalloffStart;

	if (spotLightData_->cosFalloffStart <= spotLightData_->cosAngle) {
		spotLightData_->cosFalloffStart = spotLightData_->cosAngle + 0.01f;
	}

	parameter_.SaveAndLoad();
}

void SpotLight::Parameter::Debug_Gui() {
	ImGui::ColorEdit4("color", &color.r);
	ImGui::DragFloat3("position", &position.x, 0.1f);
	ImGui::DragFloat("intensity", &intensity, 0.1f, 0.0f, 1.0f);
	ImGui::DragFloat3("direction", &direction.x, 0.1f, -1.0f, 1.0f);
	ImGui::DragFloat("distance", &distance, 0.1f, 0.0f, 10.0f);
	ImGui::DragFloat("decay", &decay, 0.1f, 0.0f, 1.0f);
}
