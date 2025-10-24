#include "BaseCamera.h"
#include "Engine/Render.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/Lib/Math/MyMatrix.h"

BaseCamera::~BaseCamera() {Finalize();}
void BaseCamera::Finalize() {}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseCamera::Init() {
	const float windowWidth = static_cast<float>(WinApp::sWindowWidth);
	const float windowHeight = static_cast<float>(WinApp::sWindowHeight);

	transform_.scale = Vector3(1, 1, 1);
	transform_.rotate = parameter_.rotate;
	transform_.translate = parameter_.translate;

	// worldの生成
	cameraMatrix_ = transform_.MakeAffine();

	viewMatrix_ = Inverse(cameraMatrix_);
	projectionMatrix_ = Matrix4x4::MakePerspectiveFov(fovY_, windowWidth / windowHeight, near_, far_);
	viewportMatrix_ = Matrix4x4::MakeViewport(0, 0, windowWidth, windowHeight, 0, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseCamera::Update() {
	const float windowWidth = static_cast<float>(WinApp::sWindowWidth);
	const float windowHeight = static_cast<float>(WinApp::sWindowHeight);

	cameraMatrix_ = transform_.MakeAffine();
	viewMatrix_ = Inverse(cameraMatrix_);

	billBordMat_ = transform_.rotate.MakeMatrix();

	projectionMatrix_ = Matrix4x4::MakePerspectiveFov(fovY_, windowWidth / windowHeight, near_, far_);

	Render::SetVpvpMatrix(this->GetVpvpMatrix());
}

//================================================================================================//
//
// アクセッサ
//
//================================================================================================//

const Matrix4x4 BaseCamera::GetVpvpMatrix() const {
	Matrix4x4 matViewProjection = viewMatrix_ * projectionMatrix_;
	return matViewProjection * viewportMatrix_;
}

const Matrix4x4 BaseCamera::GetVPVMatrix() const {
	return viewMatrix_ * projectionMatrix_ * viewportMatrix_;
}

const Vector3 BaseCamera::GetWorldPosition() const {
	Matrix4x4 matViewInverse = Inverse(viewMatrix_);
	return { matViewInverse.m[3][0], matViewInverse.m[3][1] ,matViewInverse.m[3][2] };
}
