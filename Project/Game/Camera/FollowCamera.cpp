#include "FollowCamera.h"
#include "Engine/Render.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/Lib/Json/JsonItems.h"

#include "Game/Actor/Player/Player.h"

void FollowCamera::Finalize() {
}

void FollowCamera::Init() {
	BaseCamera::Init();

	parameter_.FromJson(JsonItems::GetData("FollowCamera", "FollowCamera"));

	transform_.rotate = parameter_.rotate;
	transform_.translate = parameter_.translate;

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, "FollowCamera");
#endif // _DEBUG
}

void FollowCamera::Update() {
	if (!pTarget_) {
		return;
	}

	transform_.rotate = Quaternion::AngleAxis(angle_.x, Vector3::UP()) * Quaternion::AngleAxis(angle_.y, Vector3::RIGHT());
	transform_.translate = pTarget_->GetTransform()->translate_ + offset_;

	BaseCamera::Update();

	// renderの更新
	Render::SetEyePos(GetWorldPosition());
	Render::SetViewProjection(viewMatrix_, projectionMatrix_);
}

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

void FollowCamera::RotateCamera() {

}

Quaternion FollowCamera::GetAngleX() {
	return Quaternion::AngleAxis(angle_.x, Vector3::UP());
}
