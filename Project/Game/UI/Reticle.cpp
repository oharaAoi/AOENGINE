#include "Reticle.h"
#include "Engine.h"
#include "Lib/Math/Vector2.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Reticle::Init() {
	defaultPosition_ = Vector2{ 640.0f, 400.0f };

	reticle_ = Engine::CreateSprite("lockOffReticle.png");
	reticle_->SetTranslate(defaultPosition_);

	isLockOn_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Reticle::Update(const Matrix4x4& bossMat, const Matrix4x4& vpvpMat) {
	reticle_->Update();

	if (Input::GetInstance()->GetIsPadTrigger(XInputButtons::BUTTON_Y)) {
		LockOn();
	}

	if (isLockOn_) {
		targetMat_ = bossMat;
		reticle_->SetTranslate(WorldToScreenCoordinate(bossMat, vpvpMat));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Reticle::Draw() const {
	reticle_->Draw();
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
