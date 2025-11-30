#include "WeaponRemainingRounds.h"
#include "Engine.h"
#include "Engine/Lib/GameTimer.h"

void WeaponRemainingRounds::Init(const std::string& _name) {
	SetName(_name);
	Canvas2d* canvas = Engine::GetCanvas2d();
	gauge_ = canvas->AddSprite("white.png", "WeaponRemainingRounds");
	gauge_->ReSetTexture("white.png");
	gauge_->Load("PlayerUIs", _name);
	
	colorAnimation_.Init(Color::red, Color::white, kAnimationDuration_, (int)EasingType::None::Liner, LoopType::Return);
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
