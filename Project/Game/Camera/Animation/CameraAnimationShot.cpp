#include "CameraAnimationShot.h"
#include "Game/Camera/FollowCamera.h"
#include "Engine/Lib/GameTimer.h"

CameraAnimationShot::CameraAnimationShot() = default;
CameraAnimationShot::~CameraAnimationShot() = default;

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CameraAnimationShot::Init() {
	shotAnimation_.Load();
	isFinish_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CameraAnimationShot::Update() {
	if (shotAnimation_.isExecute) {
		ShotAnimation();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CameraAnimationShot::Debug_Gui() {
	if (ImGui::CollapsingHeader("ShotAnimation")) {
		shotAnimation_.Debug_Gui();
	}
}

void CameraAnimationShot::ShotAnimationParam::Debug_Gui() {
	ImGui::Checkbox("isExecute", &isExecute);
	ImGui::Checkbox("isApproach", &isApproach);
	// offsetZ: editor, dragFloat, 0.1
	ImGui::DragFloat("offsetZ", &offsetZ, 0.1f);
	// time: editor, dragFloat, 0.1
	ImGui::DragFloat("time", &time, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行を呼ぶ処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CameraAnimationShot::CallExecute(bool _isRevers) {
	shotAnimation_.isExecute = true;
	shotAnimation_.isApproach = _isRevers;
	shotAnimation_.timer = 0.0f;
	offset_ = pFollowCamera_->GetInitOffset();
	isFinish_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ animationの内容
///////////////////////////////////////////////////////////////////////////////////////////////

void CameraAnimationShot::ShotAnimation() {
	if (pFollowCamera_ == nullptr) { return ; }
	if (!shotAnimation_.isExecute) { return ; }

	// 時間計測
	shotAnimation_.timer += GameTimer::DeltaTime();
	float t = shotAnimation_.timer / shotAnimation_.time;

	// カメラを近づける
	float z = 0;
	if (shotAnimation_.isApproach) {
		z = std::lerp(offset_.z, shotAnimation_.offsetZ, t);
	} else {
		z = std::lerp(shotAnimation_.offsetZ, offset_.z, t);
	}
	pFollowCamera_->SetOffset(Math::Vector3(offset_.x, offset_.y, z));

	// 終了処理
	if (shotAnimation_.timer >= shotAnimation_.time) {
		shotAnimation_.isExecute = false;
		isFinish_ = true;
	}
}
