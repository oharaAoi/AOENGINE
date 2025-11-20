#include "TutorialMissionGauge.h"
#include "Engine.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/System/Editer/Window/EditorWindows.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialMissionGauge::Init() {
	Canvas2d* canvas = Engine::GetCanvas2d();
	gauge_ = canvas->AddSprite("gauge.png", "gauge");
	gaugeBg_ = canvas->AddSprite("gauge_frame.png", "gaugeBg");
	success_ = canvas->AddSprite("success.png", "succes");
	control_ = canvas->AddSprite("tutorial_move.png", "control");
	controlBg_ = canvas->AddSprite("white.png", "controlBg", "Sprite_Normal.json", -1);

	gauge_->Load("Mission", "gauge");
	gaugeBg_->Load("Mission", "gauge_frame");
	success_->Load("Mission", "success");
	control_->Load("Mission", "control");
	controlBg_->Load("Mission", "controlBg");
	success_->SetEnable(false);

	isSuccessFinish_ = false;

	AddChild(gauge_);
	AddChild(gaugeBg_);
	AddChild(success_);
	AddChild(control_);
	AddChild(controlBg_);

	alphaAnimation_.Init(1.0f, 0.0f, 1.0f, (int)EasingType::None::Liner, LoopType::Stop);

	EditorWindows::AddObjectWindow(this, "TutorialMissionGauge");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialMissionGauge::Debug_Gui() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 割合の更新
///////////////////////////////////////////////////////////////////////////////////////////////

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
