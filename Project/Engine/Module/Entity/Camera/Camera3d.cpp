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

	transform_.rotate = parameter_.rotate;
	transform_.translate = parameter_.translate;

	parameter_.SaveAndLoad();

	projectionMatrix_ = Math::Matrix4x4::MakePerspectiveFov(fovY_, float(AOENGINE::WinApp::sClientWidth) / float(AOENGINE::WinApp::sClientHeight), near_, far_);
}
