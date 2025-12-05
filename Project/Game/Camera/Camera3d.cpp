#include "Camera3d.h"
#include "Engine/Render.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/WinApp/WinApp.h"

Camera3d::Camera3d() {}
Camera3d::~Camera3d() {}
void Camera3d::Finalize() {}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Camera3d::Init() {
	BaseCamera::Init();
	parameter_.SetGroupName("Camera");
	parameter_.SetGroupName("camera3d");
	parameter_.Load();

	transform_.rotate = parameter_.rotate;
	transform_.translate = parameter_.translate;

	EditorWindows::AddObjectWindow(this, "camera3d");
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Camera3d::Update() {
	BaseCamera::Update();

	// renderの更新
	AOENGINE::Render::SetEyePos(GetWorldPosition());
	AOENGINE::Render::SetViewProjection(viewMatrix_, projectionMatrix_);
	AOENGINE::Render::SetCameraRotate(transform_.rotate);
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

	projectionMatrix_ = Math::Matrix4x4::MakePerspectiveFov(fovY_, float(WinApp::sWindowWidth) / float(WinApp::sWindowHeight), near_, far_);
}
