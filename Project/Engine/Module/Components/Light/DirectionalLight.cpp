#include "DirectionalLight.h"
#include "Engine/Lib/Json//JsonItems.h"

DirectionalLight::DirectionalLight() {
}

DirectionalLight::~DirectionalLight() {
}

void DirectionalLight::Init(ID3D12Device* device, const size_t& size) {
	AttributeGui::SetName("Directional Light");
	BaseLight::Init(device, size);
	lightBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));

	directionalLightData_->color = parameter_.color;
	directionalLightData_->direction = parameter_.direction;
	directionalLightData_->intensity = parameter_.intensity;
	directionalLightData_->limPower = parameter_.limPower;

	parameter_.FromJson(JsonItems::GetData("Light", "directionalLight"));
	baseParameter_.FromJson(JsonItems::GetData("Light", "BaseDirectional"));
	parameter_.direction = Normalize(parameter_.direction);
	directionalLightData_->color = parameter_.color;
	directionalLightData_->direction = baseParameter_.direction;
	directionalLightData_->intensity = parameter_.intensity;
	directionalLightData_->limPower = parameter_.limPower;
}

void DirectionalLight::Finalize() {
	BaseLight::Finalize();
}

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

void DirectionalLight::Draw(ID3D12GraphicsCommandList* commandList, const uint32_t& rootParameterIndex) {
	BaseLight::Draw(commandList, rootParameterIndex);
}

void DirectionalLight::Debug_Gui() {
	if (ImGui::CollapsingHeader("Base")) {
		EditParameter("BaseDirectional");
	}
	
	if (ImGui::CollapsingHeader("Unique")) {
		ImGui::ColorEdit4("color", &parameter_.color.x);
		ImGui::DragFloat("intensity", &parameter_.intensity, 0.1f, 0.0f, 1.0f);
		ImGui::DragFloat("limPower", &parameter_.limPower, 0.1f, 0.0f, 10.0f);

		if (ImGui::Button("Save")) {
			JsonItems::Save("Light", parameter_.ToJson("directionalLight"));
		}
		if (ImGui::Button("Apply")) {
			parameter_.FromJson(JsonItems::GetData("Light", "directionalLight"));
		}
	}
	parameter_.direction = Normalize(parameter_.direction);

	directionalLightData_->color = parameter_.color;
	directionalLightData_->direction = baseParameter_.direction;
	directionalLightData_->intensity = parameter_.intensity;
	directionalLightData_->limPower = parameter_.limPower;
}

void DirectionalLight::Reset() {
	baseParameter_.FromJson(JsonItems::GetData("Light", "BaseDirectional"));
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
