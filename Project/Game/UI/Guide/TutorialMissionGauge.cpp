#include "TutorialMissionGauge.h"
#include "Engine.h"
#include "Engine/System/Editer/Window/EditorWindows.h"

void TutorialMissionGauge::Init() {
	gauge_ = Engine::CreateSprite("gauge.png");
	gaugeBg_ = Engine::CreateSprite("gauge_frame.png");
	success_ = Engine::CreateSprite("success.png");
	control_ = Engine::CreateSprite("tutorial_move.png");
	controlBg_ = Engine::CreateSprite("white.png");

	gauge_->Load("Mission", "gauge");
	gaugeBg_->Load("Mission", "gauge_frame");
	success_->Load("Mission", "success");
	control_->Load("Mission", "control");
	controlBg_->Load("Mission", "controlBg");
	success_->SetEnable(false);

	isSuccessFinish_ = false;

	AddChild(gauge_.get());
	AddChild(gaugeBg_.get());
	AddChild(success_.get());
	AddChild(control_.get());
	AddChild(controlBg_.get());

	Engine::GetCanvas2d()->AddSprite(gauge_.get());
	Engine::GetCanvas2d()->AddSprite(gaugeBg_.get());
	Engine::GetCanvas2d()->AddSprite(success_.get());
	Engine::GetCanvas2d()->AddSprite(controlBg_.get());
	Engine::GetCanvas2d()->AddSprite(control_.get());

	alphaAnimation_.Init(1.0f, 0.0f, 1.0f, (int)EasingType::None::Liner, LoopType::STOP);

	EditorWindows::AddObjectWindow(this, "TutorialMissionGauge");
}

void TutorialMissionGauge::Update() {
	if (success_->GetEnable()) {
		alphaAnimation_.Update(GameTimer::DeltaTime());
		success_->SetColor(Color(1,1,1, alphaAnimation_.GetValue()));

		if (alphaAnimation_.GetIsFinish()) {
			success_->SetEnable(false);
			alphaAnimation_.Reset();
			isSuccessFinish_ = true;
		}
	}
}

void TutorialMissionGauge::Debug_Gui() {
}

void TutorialMissionGauge::FillAmountGauge(float _fillAmount) {
	gauge_->FillAmount(_fillAmount);
}

void TutorialMissionGauge::Success(bool _isSuccess) {
	isSuccessFinish_ = false;
	success_->SetEnable(_isSuccess);
}

void TutorialMissionGauge::ChangeControlUI(const std::string& _fileName) {
	control_->ReSetTexture(_fileName);
}
