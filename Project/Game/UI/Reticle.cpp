#include "Reticle.h"
#include "Engine/Core/Engine.h"
#include "Lib/Math/Vector2.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Reticle::Init() {
	defaultPosition_ = Math::Vector2((float)AOENGINE::WinApp::sClientWidth * 0.5f, (float)AOENGINE::WinApp::sClientHeight * 0.5f);

	reticle_ = Engine::GetCanvas2d()->AddSprite("lockOffReticle.png", "reticle");
	reticle_->Resize();
	reticle_->SetSaveTranslate(defaultPosition_);
	reticle_->SetTranslate(defaultPosition_);

	lockOnButton_ = Engine::GetCanvas2d()->AddSprite("lockOnButton.png", "lockOnButton");
	lockOnButton_->Resize();
	lockOnButton_->Load("UI", "lockOnButton");

	isLockOn_ = false;

	reticleParam_.Load();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Reticle::Update() {
	defaultPosition_ = Math::Vector2((float)AOENGINE::WinApp::sClientWidth * 0.5f, (float)AOENGINE::WinApp::sClientHeight * 0.5f);

	reticle_->Update();
	lockOnButton_->Update();

	reticlePos_ = reticle_->GetTranslate();
	lockOnButton_->SetTranslate(Math::Vector2(reticlePos_.x, reticlePos_.y + reticleParam_.offsetY));
}

void Reticle::LockOn() {
	if (isLockOn_) {
		reticle_->SetTranslate(defaultPosition_);
		reticle_->ReSetTexture("lockOffReticle.png");
		isLockOn_ = false;
	} else {
		reticle_->ReSetTexture("lockOnReticle.png");
		isLockOn_ = true;
	}
}

void Reticle::Debug_Gui() {
	reticleParam_.Debug_Gui();
}

void Reticle::ReticleParameter::Debug_Gui() {
	ImGui::DragFloat("offsetY", &offsetY, 1);
	SaveAndLoad();
}

void Reticle::SetReticlePos(AOENGINE::WorldTransform* targetTransform, const Math::Matrix4x4& vpvpMat) {
	if (isLockOn_) {
		targetTransform_ = targetTransform;
		reticle_->SetTranslate(WorldToScreenCoordinate(targetTransform->GetWorldMatrix(), vpvpMat));
	}
}

void Reticle::ReleaseLockOn() {
	reticle_->SetTranslate(defaultPosition_);
	reticle_->ReSetTexture("lockOffReticle.png");
	isLockOn_ = false;
}

Math::Vector3 Reticle::GetTargetPos() const {
	if (targetTransform_) {
		return targetTransform_->GetWorldPos();
	} else {
		return CVector3::ZERO;
	}
}
