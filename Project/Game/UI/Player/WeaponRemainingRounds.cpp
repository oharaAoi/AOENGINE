#include "WeaponRemainingRounds.h"
#include "Engine.h"

void WeaponRemainingRounds::Init(const std::string& _name) {
	SetName(_name);
	gauge_ = Engine::CreateSprite("white.png");
	gauge_->Load("PlayerUIs", _name);
	gauge_->ReSetTexture("white.png");
	Engine::GetCanvas2d()->AddSprite(gauge_.get());

	colorAnimation_.Init(Color::red, Color::white, .4f, (int)EasingType::None::Liner, LoopType::RETURN);
	timer_ = 0;
}

void WeaponRemainingRounds::Update(const Vector2& pos, float _fillAmount) {
	gauge_->SetColor({ 1,1,1,1 });
	gauge_->SetTranslate(pos);
	gauge_->FillAmount(_fillAmount);
}

void WeaponRemainingRounds::Debug_Gui() {
	gauge_->Debug_Gui();
}

void WeaponRemainingRounds::Blinking() {
	colorAnimation_.Update(GameTimer::DeltaTime());
	gauge_->SetColor(colorAnimation_.GetValue());
}
