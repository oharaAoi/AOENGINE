#include "CameraAnimation.h"
#include "Game/Camera/FollowCamera.h"
#include "Engine/Lib/GameTimer.h"

CameraAnimation::CameraAnimation() = default;
CameraAnimation::~CameraAnimation() = default;

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CameraAnimation::Init() {
	shotAnimation_.Load();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CameraAnimation::Update() {
	if (shotAnimation_.isExecute) {
		ShotAnimation();
	}
}

void CameraAnimation::Debug_Gui() {
	if (ImGui::CollapsingHeader("ShotAnimation")) {
		shotAnimation_.Debug_Gui();
	}
}

void CameraAnimation::ShotAnimationParam::Debug_Gui() {
	ImGui::Checkbox("isExecute", &isExecute);
	ImGui::Checkbox("isApproach", &isApproach);
	// offsetZ: editor, dragFloat, 0.1
	ImGui::DragFloat("offsetZ", &offsetZ, 0.1f);
	// time: editor, dragFloat, 0.1
	ImGui::DragFloat("time", &time, 0.1f);
	SaveAndLoad();
}

void CameraAnimation::ExecuteShotAnimation(bool _isApproach) {
	shotAnimation_.isExecute = true;
	shotAnimation_.isApproach = _isApproach;
	shotAnimation_.timer = 0.0f;
	offset_ = pFollowCamera_->GetInitOffset();
}

void CameraAnimation::ShotAnimation() {
	if (pFollowCamera_ == nullptr) { return ; }
	if (!shotAnimation_.isExecute) { return ; }

	shotAnimation_.timer += GameTimer::DeltaTime();
	float t = shotAnimation_.timer / shotAnimation_.time;
	float z = 0;
	if (shotAnimation_.isApproach) {
		z = std::lerp(offset_.z, shotAnimation_.offsetZ, t);
	} else {
		z = std::lerp(shotAnimation_.offsetZ, offset_.z, t);
	}
	pFollowCamera_->SetOffset(Vector3(offset_.x, offset_.y, z));

	if (shotAnimation_.timer >= shotAnimation_.time) {
		shotAnimation_.isExecute = false;
	}
}
