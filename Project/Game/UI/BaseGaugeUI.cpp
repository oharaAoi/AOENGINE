#include "BaseGaugeUI.h"
#include "Engine.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseGaugeUI::Init(const std::string& bgTexture, const std::string& frontTexture) {
	Canvas2d* canvas = Engine::GetCanvas2d();
	bg_ = canvas->AddSprite(bgTexture, "bg");
	front_ = canvas->AddSprite(frontTexture, "front");
	
	fillAmount_ = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseGaugeUI::Update() {
	Vector2 centerPos = front_->GetTranslate();
	Vector2 scale = front_->GetScale();

	front_->FillAmount(fillAmount_);
	
	bg_->SetTranslate(centerPos);
	front_->SetTranslate(centerPos);

	bg_->SetScale(scale);
	front_->SetScale(scale);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseGaugeUI::Debug_Gui() {
	ImGui::DragFloat("FillAmount", &fillAmount_, 0.01f);

	if (ImGui::CollapsingHeader("Front")) {
		front_->Debug_Gui();
	}
	if (ImGui::CollapsingHeader("Bg")) {
		bg_->Debug_Gui();
	}
}

void BaseGaugeUI::SetIsEnable(bool _isActive) {
	bg_->SetEnable(_isActive);
	front_->SetEnable(_isActive);
}

bool BaseGaugeUI::GetIsEnable() const {
	return bg_->GetEnable();
}

void BaseGaugeUI::SetPos(const Vector2& _pos) {
	bg_->SetTranslate(_pos);
	front_->SetTranslate(_pos);
}
