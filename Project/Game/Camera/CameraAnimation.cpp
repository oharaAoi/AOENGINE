#include "CameraAnimation.h"

void CameraAnimation::Init() {
	timer_ = 0.0f;
}

void CameraAnimation::Update() {

}

void CameraAnimation::ShotAnimationParam::Debug_Gui() {
	ImGui::Checkbox("isExecute", &isExcute);
	ImGui::Checkbox("isApproach", &isApproach);
	// offsetZ: editor, dragFloat, 0.1
	ImGui::DragFloat("offsetZ", &offsetZ, 0.1f);
	// time: editor, dragFloat, 0.1
	ImGui::DragFloat("time", &time, 0.1f);
	SaveAndLoad();
}

void CameraAnimation::ExecuteShotAnimation(bool _isApproach) {
	shotAnimation_.isExcute = true;
	shotAnimation_.isApproach = _isApproach;
}

void CameraAnimation::ShotAnimation() {
	
}
