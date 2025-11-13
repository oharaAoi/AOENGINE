#include "DirectionalLight.h"
#include "Engine/Lib/Json//JsonItems.h"

DirectionalLight::DirectionalLight() {
}

DirectionalLight::~DirectionalLight() {
}

void DirectionalLight::Finalize() {
	BaseLight::Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void DirectionalLight::Init(ID3D12Device* device, const size_t& size) {
	AttributeGui::SetName("Directional Light");
	BaseLight::Init(device, size);
	lightBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));

	directionalLightData_->color = parameter_.color;
	directionalLightData_->direction = parameter_.direction;
	directionalLightData_->intensity = parameter_.intensity;
	directionalLightData_->limPower = parameter_.limPower;

	parameter_.SetGroupName("Light");

	parameter_.FromJson(JsonItems::GetData("Light", "directionalLight"));
	baseParameter_.FromJson(JsonItems::GetData("Light", "BaseDirectional"));
	parameter_.direction = Normalize(parameter_.direction);
	directionalLightData_->color = parameter_.color;
	directionalLightData_->direction = baseParameter_.direction;
	directionalLightData_->intensity = parameter_.intensity;
	directionalLightData_->limPower = parameter_.limPower;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void DirectionalLight::Update() {
	parameter_.direction = Normalize(parameter_.direction);

	directionalLightData_->color = parameter_.color;
	directionalLightData_->direction = baseParameter_.direction;
	directionalLightData_->intensity = parameter_.intensity;
	directionalLightData_->limPower = parameter_.limPower;

	directionalLightData_->direction = Normalize(directionalLightData_->direction);
	direction_ = directionalLightData_->direction;
	CalucViewProjection(lightPos_);
	directionalLightData_->viewProjection = viewProjectionMatrix_;
	BaseLight::Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void DirectionalLight::BindCommand(ID3D12GraphicsCommandList* commandList, const uint32_t& rootParameterIndex) {
	BaseLight::BindCommand(commandList, rootParameterIndex);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 再読み込み処理
///////////////////////////////////////////////////////////////////////////////////////////////

void DirectionalLight::Reset() {
	baseParameter_.Load();
	parameter_.FromJson(JsonItems::GetData("Light", "directionalLight"));

	parameter_.direction = Normalize(parameter_.direction);
	directionalLightData_->color = parameter_.color;
	directionalLightData_->direction = baseParameter_.direction;
	directionalLightData_->intensity = parameter_.intensity;
	directionalLightData_->limPower = parameter_.limPower;

	baseParameter_.direction = baseParameter_.direction.Normalize();

	lightPos_ = baseParameter_.lightPos;
	direction_ = baseParameter_.direction;
	fovY_ = baseParameter_.fovY;
	near_ = baseParameter_.nearClip;
	far_ = baseParameter_.farClip;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void DirectionalLight::Debug_Gui() {
	if (ImGui::CollapsingHeader("Base")) {
		EditParameter("BaseDirectional");
	}

	if (ImGui::CollapsingHeader("Unique")) {
		parameter_.Debug_Gui();
	}
	parameter_.direction = Normalize(parameter_.direction);

	directionalLightData_->color = parameter_.color;
	directionalLightData_->direction = baseParameter_.direction;
	directionalLightData_->intensity = parameter_.intensity;
	directionalLightData_->limPower = parameter_.limPower;
}

void DirectionalLight::Paramter::Debug_Gui() {
	ImGui::ColorEdit4("color", &color.r);
	ImGui::DragFloat("intensity", &intensity, 0.1f, 0.0f, 1.0f);
	ImGui::DragFloat("limPower", &limPower, 0.1f, 0.0f, 10.0f);

	SaveAndLoad();
}
