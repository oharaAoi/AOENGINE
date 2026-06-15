#include "LightInspector.h"

#include "Engine/System/Manager/ImGuiManager.h"

#include <numbers>

using namespace AOENGINE;

//////////////////////////////////////////////////////////////////////////////////////////////////
// Light共通パラメータを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseLightInspector::DrawBaseParameters(BaseLight& light, const char* label) {
	if (!ImGui::CollapsingHeader(label)) {
		return;
	}

	BaseLight::BaseParameter& parameter = light.GetBaseParameter();
	ImGui::DragFloat3("pos", &parameter.lightPos.x, 0.1f);
	ImGui::DragFloat3("direction", &parameter.direction.x, 0.1f);
	ImGui::DragFloat("fovY", &parameter.fovY, 0.1f);
	ImGui::DragFloat("nearClip", &parameter.nearClip, 0.1f);
	ImGui::DragFloat("farClip", &parameter.farClip, 0.1f);
	ImGui::DragFloat("shadowDepth", &parameter.shadowDepth, 0.1f);
	ImGui::DragFloat("shadowWidth", &parameter.shadowWidth, 0.1f);
	ImGui::DragFloat("shadowHeight", &parameter.shadowHeight, 0.1f);
	parameter.SaveAndLoad();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// LightGroupのInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void LightGroupInspector::Draw(LightGroup& lightGroup) {
	if (DirectionalLight* light = lightGroup.GetDirectionalLight()) {
		if (ImGui::TreeNode("Directional Light")) {
			DirectionalLightInspector::Draw(*light);
			ImGui::TreePop();
		}
	}

	if (PointLight* light = lightGroup.GetPointLight()) {
		if (ImGui::TreeNode("Point Light")) {
			PointLightInspector::Draw(*light);
			ImGui::TreePop();
		}
	}

	if (SpotLight* light = lightGroup.GetSpotLight()) {
		if (ImGui::TreeNode("Spot Light")) {
			SpotLightInspector::Draw(*light);
			ImGui::TreePop();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// DirectionalLightのInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void DirectionalLightInspector::Draw(DirectionalLight& light) {
	BaseLightInspector::DrawBaseParameters(light, "Base Directional");

	if (!ImGui::CollapsingHeader("Directional")) {
		return;
	}

	DirectionalLight::Paramter& parameter = light.GetParameter();
	ImGui::ColorEdit4("color", &parameter.color.r);
	ImGui::DragFloat("intensity", &parameter.intensity, 0.1f, 0.0f, 1.0f);
	ImGui::DragFloat("limPower", &parameter.limPower, 0.1f, 0.0f, 10.0f);
	parameter.SaveAndLoad();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// PointLightのInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void PointLightInspector::Draw(PointLight& light) {
	BaseLightInspector::DrawBaseParameters(light, "Base Point");

	if (!ImGui::CollapsingHeader("Point")) {
		return;
	}

	PointLight::Paramter& parameter = light.GetParameter();
	ImGui::ColorEdit4("color", &parameter.color.r);
	ImGui::DragFloat3("position", &parameter.position.x, 0.1f);
	ImGui::DragFloat("intensity", &parameter.intensity, 0.1f, 0.0f, 1.0f);
	ImGui::DragFloat("radius", &parameter.radius, 0.1f, 0.0f, 10.0f);
	ImGui::DragFloat("decay", &parameter.decay, 0.1f, 0.0f, 1.0f);
	parameter.SaveAndLoad();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// SpotLightのInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void SpotLightInspector::Draw(SpotLight& light) {
	BaseLightInspector::DrawBaseParameters(light, "Base Spot");

	if (!ImGui::CollapsingHeader("Spot")) {
		return;
	}

	SpotLight::Parameter& parameter = light.GetParameter();
	ImGui::ColorEdit4("color", &parameter.color.r);
	ImGui::DragFloat3("position", &parameter.position.x, 0.1f);
	ImGui::DragFloat("intensity", &parameter.intensity, 0.1f, 0.0f, 1.0f);
	ImGui::DragFloat3("direction", &parameter.direction.x, 0.1f, -1.0f, 1.0f);
	ImGui::DragFloat("distance", &parameter.distance, 0.1f, 0.0f, 10.0f);
	ImGui::DragFloat("decay", &parameter.decay, 0.1f, 0.0f, 1.0f);

	ImGui::SliderFloat("cosAngle", &light.GetCosDegree(), 0.0f, std::numbers::pi_v<float>);
	ImGui::SliderFloat("falloffDegree", &light.GetFalloffDegree(), 0.0f, std::numbers::pi_v<float>);

	parameter.SaveAndLoad();
}
