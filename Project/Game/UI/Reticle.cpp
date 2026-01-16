#include "Reticle.h"
#include "Engine.h"
#include "Lib/Math/Vector2.h"
#include "Engine/System/Input/Input.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Reticle::Init() {
	defaultPosition_ = Math::Vector2((float)AOENGINE::WinApp::sClientWidth * 0.5f, (float)AOENGINE::WinApp::sClientHeight * 0.5f);

	reticle_ = Engine::GetCanvas2d()->AddSprite("lockOffReticle.png", "reticle");
	reticle_->Resize();
	reticle_->SetTranslate(defaultPosition_);

	isLockOn_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Reticle::Update(const Math::Matrix4x4& bossMat, const Math::Matrix4x4& vpvpMat) {
	defaultPosition_ = Math::Vector2((float)AOENGINE::WinApp::sClientWidth * 0.5f, (float)AOENGINE::WinApp::sClientHeight * 0.5f);

	reticle_->Update();

	if (AOENGINE::Input::GetInstance()->IsTriggerButton(XInputButtons::RStickThumb)) {
		LockOn();
	}

	if (AOENGINE::Input::GetInstance()->GetKey(DIK_L)) {
		LockOn();
	}

	if (isLockOn_) {
		targetMat_ = bossMat;
		reticle_->SetTranslate(WorldToScreenCoordinate(bossMat, vpvpMat));
	}

	reticlePos_ = reticle_->GetTranslate();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Reticle::Draw() const {
	AOENGINE::Pipeline* pso = Engine::GetLastUsedPipeline();
	reticle_->Draw(pso);
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

void Reticle::ReleaseLockOn() {
	reticle_->SetTranslate(defaultPosition_);
	reticle_->ReSetTexture("lockOffReticle.png");
	isLockOn_ = false;
}
