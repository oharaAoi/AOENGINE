#include "WeaponRemainingRounds.h"
#include "Engine.h"

void WeaponRemainingRounds::Init(const std::string& _name) {
	SetName(_name);
	gauge_ = Engine::CreateSprite("white.png");
	gauge_->Load("PlayerUIs", _name);
	gauge_->ReSetTexture("white.png");
	Engine::GetCanvas2d()->AddSprite(gauge_.get());
}

void WeaponRemainingRounds::Update(const Vector2& pos, float _fillAmount) {
	gauge_->SetTranslate(pos);
	gauge_->FillAmount(_fillAmount);
}

void WeaponRemainingRounds::Debug_Gui() {
	gauge_->Debug_Gui();
	if (ImGui::Button("Save")) {
		gauge_->Save("PlayerUIs", GetName());
	}
}
