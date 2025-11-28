#include "Reticle.h"
#include "Engine.h"
#include "Lib/Math/Vector2.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Reticle::Init() {
	defaultPosition_ = Vector2((float)WinApp::sWindowWidth * 0.5f, (float)WinApp::sWindowHeight * 0.5f);

	reticle_ = Engine::GetCanvas2d()->AddSprite("lockOffReticle.png", "reticle");
	reticle_->SetTranslate(defaultPosition_);

	isLockOn_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Reticle::Update(const Matrix4x4& bossMat, const Matrix4x4& vpvpMat) {
	reticle_->Update();

	if (Input::GetInstance()->IsTriggerButton(XInputButtons::RStickThumb)) {
		LockOn();
	}

	if (Input::GetInstance()->GetKey(DIK_L)) {
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
	Pipeline* pso = Engine::GetLastUsedPipeline();
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
