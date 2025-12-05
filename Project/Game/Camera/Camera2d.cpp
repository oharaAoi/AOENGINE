#include "Camera2d.h"
#include "Engine/Render.h"
#include "Engine/WinApp/WinApp.h"

Camera2d::Camera2d() {}
Camera2d::~Camera2d() {}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Camera2d::Init() {
	const float windowWidth = static_cast<float>(WinApp::sWindowWidth);
	const float windowHeight = static_cast<float>(WinApp::sWindowHeight);

	translate_ = CVector3::ZERO;
	projectionMatrix_ = Math::Matrix4x4::MakeOrthograhic(0.0f, 0.0f, windowWidth, windowHeight, near_, far_);
	viewMatrix_ = Math::Matrix4x4::MakeUnit();

	viewportMatrix_ = Math::Matrix4x4::MakeViewport(0, 0, windowWidth, windowHeight, 0, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Camera2d::Update() {
	// 4x4で表現するためz成分を削除
	translate_.z = 0;
	viewMatrix_ = Inverse(translate_.MakeTranslateMat());
	worldMat_ = Multiply(Multiply(Math::Matrix4x4::MakeUnit(), Math::Matrix4x4::MakeUnit()), translate_.MakeTranslateMat());
	// Renderに設定
	AOENGINE::Render::SetViewProjection2D(viewMatrix_, projectionMatrix_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Camera2d::Debug_Gui() {
	const uint32_t windowWidth = WinApp::sWindowWidth;
	const uint32_t windowHeight = WinApp::sWindowHeight;

	ImGui::DragFloat("near", &near_, 0.1f);
	ImGui::DragFloat("far", &far_, 0.1f);
	ImGui::DragFloat2("translate", &translate_.x, 1.0f);

	projectionMatrix_ = Math::Matrix4x4::MakeOrthograhic(0.0f, 0.0f, float(windowWidth), float(windowHeight), near_, far_);
}