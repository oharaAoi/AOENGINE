#include "BaseGaugeUI.h"
#include "Engine.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseGaugeUI::Init(const std::string& bgTexture, const std::string& frontTexture) {
	bg_ = Engine::CreateSprite(bgTexture);
	front_ = Engine::CreateSprite(frontTexture);

	fillAmount_ = 1.0f;
	centerPos_ = Vector2(kWindowWidth_ * 0.5f, kWindowHeight_ * 0.5f);

	bg_->SetTranslate(centerPos_);
	front_->SetTranslate(centerPos_);

	bg_->SetScale(scale_);
	front_->SetScale(scale_);

	Engine::GetCanvas2d()->AddSprite(bg_.get());
	Engine::GetCanvas2d()->AddSprite(front_.get());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseGaugeUI::Update() {
	front_->FillAmount(fillAmount_);
	
	bg_->SetTranslate(centerPos_);
	front_->SetTranslate(centerPos_);

	bg_->SetScale(scale_);
	front_->SetScale(scale_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////
void BaseGaugeUI::Draw() const {
	
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseGaugeUI::Debug_Gui() {
	ImGui::DragFloat2("Scale", &scale_.x, 0.1f);
	ImGui::DragFloat2("CenterPos", &centerPos_.x, 0.1f);
	ImGui::DragFloat("FillAmount", &fillAmount_, 0.01f);

	bg_->SetScale(scale_);
	front_->SetScale(scale_);

	front_->Debug_Gui();
}

void BaseGaugeUI::SetIsEnable(bool _isActive) {
	bg_->SetEnable(_isActive);
	front_->SetEnable(_isActive);
}
