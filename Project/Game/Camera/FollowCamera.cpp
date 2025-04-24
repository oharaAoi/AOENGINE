#include "FollowCamera.h"
#include "Engine/Render.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/Lib/Json/JsonItems.h"

#include "Game/Actor/Player/Player.h"

void FollowCamera::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Init() {
	BaseCamera::Init();

	parameter_.FromJson(JsonItems::GetData("FollowCamera", "FollowCamera"));

	transform_.rotate = parameter_.rotate;
	transform_.translate = parameter_.translate;

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, "FollowCamera");
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Update() {
	if (!pTarget_) {
		return;
	}

	RotateCamera();

	if (pReticle_->GetLockOn()) {
		Quaternion targetToRotate = Quaternion::LookAt(transform_.translate, pReticle_->GetTargetPos());
		transform_.rotate = Quaternion::Slerp(transform_.rotate, targetToRotate, 0.2f);

		Vector3 euler = transform_.rotate.QuaternionToEuler();
		angle_.x = euler.y;
		angle_.y = euler.x;

	} else {
		transform_.rotate = Quaternion::AngleAxis(angle_.x, CVector3::UP) * Quaternion::AngleAxis(angle_.y, CVector3::RIGHT);
	}
	
	Vector3 point = pTarget_->GetTransform()->translate_ + offset_;
	Vector3 direction = transform_.rotate.Rotate({ 0.0f, 0.0f, -1.0f });

	transform_.translate = point + (direction * 20.0f);

	BaseCamera::Update();
	// renderの更新
	Render::SetEyePos(GetWorldPosition());
	Render::SetViewProjection(viewMatrix_, projectionMatrix_);
	Render::SetCameraRotate(transform_.rotate);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void FollowCamera::Debug_Gui() {
	ImGui::DragFloat("near", &near_, 0.1f);
	ImGui::DragFloat("far", &far_, 0.1f);
	ImGui::DragFloat("fovY", &fovY_, 0.1f);
	ImGui::DragFloat3("offset", &offset_.x, 0.1f);

	ImGui::DragFloat3("rotate", &parameter_.rotate.x, 0.1f);
	ImGui::DragFloat3("translate", &parameter_.translate.x, 0.1f);
	transform_.rotate = parameter_.rotate;
	transform_.translate = parameter_.translate;

	if (ImGui::Button("Save")) {
		JsonItems::Save("FollowCamera", parameter_.ToJson("FollowCamera"));
	}
	if (ImGui::Button("Apply")) {
		parameter_.FromJson(JsonItems::GetData("FollowCamera", "FollowCamera"));
	}

	projectionMatrix_ = Matrix4x4::MakePerspectiveFov(fovY_, float(kWindowWidth_) / float(kWindowHeight_), near_, far_);
}

#endif // _DEBUG
///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ カメラを回転させる
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::RotateCamera() {
	stick_ = Input::GetInstance()->GetRightJoyStick(kDeadZone_).Normalize();
	
	if (std::abs(stick_.x) > kDeadZone_) {
		angle_.x += stick_.x * rotateDelta_;
	}

	if (std::abs(stick_.y) > kDeadZone_) {
		angle_.y += stick_.y * rotateDelta_;
	}

	angle_.y = std::clamp(angle_.y, angleLimitY_.first, angleLimitY_.second);
}

Quaternion FollowCamera::GetAngleX() {
	//return Quaternion::AngleAxis(angle_.x, CVector3::UP);
	Vector3 euler = transform_.rotate.QuaternionToEuler();
	return Quaternion::AngleAxis(euler.y, CVector3::UP);
}
