#include "CameraShakeParameters.h"

#include <algorithm>
#include "Engine/System/Manager/ImGuiManager.h"

CameraShakeRequest::CameraShakeRequest() {
	SetGroupName("Camera");
	SetName("CameraShake");
}

json CameraShakeRequest::ToJson(const std::string& id) const {
	return AOENGINE::JsonBuilder(id)
		.Add("source", static_cast<int>(source))
		.Add("duration", duration)
		.Add("strength", strength)
		.Add("positionAmplitude", positionAmplitude)
		.Add("rotationAmplitude", rotationAmplitude)
		.Add("decay", static_cast<int>(decay))
		.Add("positionFrequency", positionFrequency)
		.Add("rotationFrequency", rotationFrequency)
		.Add("phase", phase)
		.Add("noiseType", static_cast<int>(noiseType))
		.Add("noiseStrength", noiseStrength)
		.Add("noiseFrequency", noiseFrequency)
		.Add("seed", seed)
		.Build();
}

void CameraShakeRequest::FromJson(const json& jsonData) {
	int sourceValue = static_cast<int>(source);
	int decayValue = static_cast<int>(decay);
	int noiseTypeValue = static_cast<int>(noiseType);

	Convert::fromJson(jsonData, "source", sourceValue);
	Convert::fromJson(jsonData, "duration", duration);
	Convert::fromJson(jsonData, "strength", strength);
	Convert::fromJson(jsonData, "positionAmplitude", positionAmplitude);
	Convert::fromJson(jsonData, "rotationAmplitude", rotationAmplitude);
	Convert::fromJson(jsonData, "decay", decayValue);
	Convert::fromJson(jsonData, "positionFrequency", positionFrequency);
	Convert::fromJson(jsonData, "rotationFrequency", rotationFrequency);
	Convert::fromJson(jsonData, "phase", phase);
	Convert::fromJson(jsonData, "noiseType", noiseTypeValue);
	Convert::fromJson(jsonData, "noiseStrength", noiseStrength);
	Convert::fromJson(jsonData, "noiseFrequency", noiseFrequency);
	Convert::fromJson(jsonData, "seed", seed);

	source = static_cast<CameraShakeSource>(std::clamp(sourceValue, 0, 1));
	decay = static_cast<CameraShakeDecay>(std::clamp(decayValue, 0, 2));
	noiseType = static_cast<CameraShakeNoiseType>(std::clamp(noiseTypeValue, 0, 2));
	duration = std::max(duration, 0.0f);
	strength = std::max(strength, 0.0f);
	noiseStrength = std::max(noiseStrength, 0.0f);
	noiseFrequency = std::max(noiseFrequency, 0.0f);
}

void CameraShakeRequest::Debug_Gui() {
	if (ImGui::CollapsingHeader("CameraShake")) {
		int testSource = static_cast<int>(source);
		if (ImGui::Combo("Source", &testSource, "Parameter\0Noise\0")) {
			source = static_cast<CameraShakeSource>(testSource);
		}

		int testDecay = static_cast<int>(decay);
		if (ImGui::Combo("Decay", &testDecay, "None\0Linear\0EaseOut\0")) {
			decay = static_cast<CameraShakeDecay>(testDecay);
		}

		ImGui::DragFloat("Duration", &duration, 0.01f, 0.0f, 60.0f);
		ImGui::DragFloat("Strength", &strength, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat3("Position Amplitude", &positionAmplitude.x, 0.01f);
		ImGui::DragFloat3("Rotation Amplitude (deg)", &rotationAmplitude.x, 0.01f);

		if (source == CameraShakeSource::Parameter) {
			ImGui::DragFloat3("Position Frequency", &positionFrequency.x, 0.1f, 0.0f, 500.0f);
			ImGui::DragFloat3("Rotation Frequency", &rotationFrequency.x, 0.1f, 0.0f, 500.0f);
			ImGui::DragFloat3("Phase", &phase.x, 0.01f);
		} else {
			int testNoiseType = static_cast<int>(noiseType);
			if (ImGui::Combo("Noise Type", &testNoiseType, "White\0Value\0Perlin\0")) {
				noiseType = static_cast<CameraShakeNoiseType>(testNoiseType);
			}
			ImGui::DragFloat("Noise Strength", &noiseStrength, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Noise Frequency", &noiseFrequency, 0.1f, 0.0f, 500.0f);
			ImGui::InputScalar("Seed", ImGuiDataType_U32, &seed);
		}
	}
}
