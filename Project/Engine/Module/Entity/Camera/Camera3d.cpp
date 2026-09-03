#include "Camera3d.h"
#include "Engine/Render/Render.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/WinApp/WinApp.h"

Camera3d::Camera3d() {}
Camera3d::~Camera3d() {}
void Camera3d::Finalize() {}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Camera3d::Init() {
	SetName("camera3d");
	BaseCamera::Init();
	parameter_.SetGroupName("Camera");
	parameter_.SetGroupName("camera3d");
	parameter_.Load();
	shakeRequest_.Load();

	transform_.rotate = parameter_.rotate;
	transform_.translate = parameter_.translate;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Camera3d::Update() {
	BaseCamera::Update();
	ApplyToRender();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Camera3d::Debug_Gui() {
	ImGui::DragFloat("near", &near_, 0.1f);
	ImGui::DragFloat("far", &far_, 0.1f);
	ImGui::DragFloat("fovY", &fovY_, 0.1f);
	ImGui::DragFloat3("rotate", &parameter_.rotate.x, 0.1f);
	ImGui::DragFloat3("translate", &parameter_.translate.x, 0.1f);

	if(ImGui::CollapsingHeader("CameraShake")) {
		int source = static_cast<int>(shakeRequest_.source);
		if (ImGui::Combo("Source", &source, "Parameter\0Noise\0")) {
			shakeRequest_.source = static_cast<CameraShakeSource>(source);
		}

		int decay = static_cast<int>(shakeRequest_.decay);
		if (ImGui::Combo("Decay", &decay, "None\0Linear\0EaseOut\0")) {
			shakeRequest_.decay = static_cast<CameraShakeDecay>(decay);
		}

		ImGui::DragFloat("Duration", &shakeRequest_.duration, 0.01f, 0.0f, 60.0f);
		ImGui::DragFloat("Strength", &shakeRequest_.strength, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat3("Position Amplitude", &shakeRequest_.positionAmplitude.x, 0.01f);
		ImGui::DragFloat3("Rotation Amplitude (deg)", &shakeRequest_.rotationAmplitude.x, 0.01f);

		if (shakeRequest_.source == CameraShakeSource::Parameter) {
			ImGui::DragFloat3("Position Frequency", &shakeRequest_.positionFrequency.x, 0.1f, 0.0f, 500.0f);
			ImGui::DragFloat3("Rotation Frequency", &shakeRequest_.rotationFrequency.x, 0.1f, 0.0f, 500.0f);
			ImGui::DragFloat3("Phase", &shakeRequest_.phase.x, 0.01f);
		} else {
			int noiseType = static_cast<int>(shakeRequest_.noiseType);
			if (ImGui::Combo("Noise Type", &noiseType, "White\0Value\0Perlin\0")) {
				shakeRequest_.noiseType = static_cast<CameraShakeNoiseType>(noiseType);
			}
			ImGui::DragFloat("Noise Strength", &shakeRequest_.noiseStrength, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Noise Frequency", &shakeRequest_.noiseFrequency, 0.1f, 0.0f, 500.0f);
			ImGui::InputScalar("Seed", ImGuiDataType_U32, &shakeRequest_.seed);
		}

		if (ImGui::Button("Test Play")) {
			PlayShake(shakeRequest_);
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop")) {
			StopShake();
		}
		ImGui::SameLine();
		ImGui::TextUnformatted(IsShaking() ? "Playing" : "Stopped");

		shakeRequest_.SaveAndLoad();
	}

	transform_.rotate = parameter_.rotate;
	transform_.translate = parameter_.translate;

	parameter_.SaveAndLoad();

	projectionMatrix_ = Math::Matrix4x4::MakePerspectiveFov(fovY_, float(AOENGINE::WinApp::sClientWidth) / float(AOENGINE::WinApp::sClientHeight), near_, far_);
}
