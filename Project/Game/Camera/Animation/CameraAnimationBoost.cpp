#include "CameraAnimationBoost.h"
#include "Engine/Lib/GameTimer.h"
#include "Game/Camera/FollowCamera.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CameraAnimationBoost::Init() {
	param_.Load();
	isFinish_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CameraAnimationBoost::Update() {
	if (param_.isExecute) {
		BoostAnimation();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行呼び出し
///////////////////////////////////////////////////////////////////////////////////////////////

void CameraAnimationBoost::CallExecute(bool _isRevers) {
	param_.isExecute = true;
	param_.isApproach = _isRevers;
	param_.timer = 0.0f;
	cameraOffset_ = pFollowCamera_->GetInitOffset();
	isFinish_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ boost時のカメラの寄り
///////////////////////////////////////////////////////////////////////////////////////////////

void CameraAnimationBoost::BoostAnimation() {
	if (pFollowCamera_ == nullptr) { return; }
	if (!param_.isExecute) { return; }

	param_.timer += GameTimer::DeltaTime();
	float t = param_.timer / param_.time;

	Vector3 offset = CVector3::ZERO;
	if (param_.isApproach) {
		offset = Vector3::Lerp(cameraOffset_, param_.offset, t);
	} else {
		offset = Vector3::Vector3::Lerp(param_.offset, cameraOffset_, t);
	}
	pFollowCamera_->SetOffset(offset);

	if (param_.timer >= param_.time) {
		param_.isExecute = false;
		isFinish_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CameraAnimationBoost::Debug_Gui() {
	if (ImGui::CollapsingHeader("Parameter")) {
		param_.Debug_Gui();
	}
}

void CameraAnimationBoost::BoostAnimationParam::Debug_Gui() {
	ImGui::Checkbox("isExecute", &isExecute);
	ImGui::Checkbox("isApproach", &isApproach);
	// offsetZ: editor, dragFloat, 0.1
	ImGui::DragFloat3("offset", &offset.x, 0.1f);
	// time: editor, dragFloat, 0.1
	ImGui::DragFloat("time", &time, 0.1f);
	SaveAndLoad();
}
