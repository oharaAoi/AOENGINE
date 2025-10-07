#include "GameModeGuide.h"
#include "Engine.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/System/Input/Input.h"

GameModeGuide::GameModeGuide() = default;
GameModeGuide::~GameModeGuide() = default;

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GameModeGuide::Init() {
	SetName("GameModeGuide");
	goGame_ = Engine::CreateSprite("goGame.png");
	goTutorial_ = Engine::CreateSprite("goTutorial.png");
	select_ = Engine::CreateSprite("gradation.png");

	goGame_->Load("Guide","goGame");
	goTutorial_->Load("Guide","goTutorial");
	
	goGame_->SetEnable(false);
	goTutorial_->SetEnable(false);
	select_->SetEnable(false);

	AddChild(goGame_.get());
	AddChild(goTutorial_.get());
	AddChild(select_.get());
	
	Engine::GetCanvas2d()->AddSprite(goGame_.get());
	Engine::GetCanvas2d()->AddSprite(goTutorial_.get());
	Engine::GetCanvas2d()->AddSprite(select_.get());

	selectModeType_ = SelectModeType::ToTutorial;
	selectModeIndex_ = 1;
	isOpen_ = false;
	isSelectCoolTime_ = false;
	timer_ = 0.0f;

	EditorWindows::AddObjectWindow(this, GetName());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GameModeGuide::Update() {
	if (!isOpen_) { return; }

	if (!isSelectCoolTime_) {
		isSelectCoolTime_ = Select();
	} else {
		timer_ += GameTimer::DeltaTime();

		if (timer_ >= 0.2f) {
			isSelectCoolTime_ = false;
			timer_ = 0.0f;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GameModeGuide::Debug_Gui() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 開く
///////////////////////////////////////////////////////////////////////////////////////////////

void GameModeGuide::Open() {
	goGame_->SetEnable(true);
	goTutorial_->SetEnable(true);
	select_->SetEnable(true);
	isOpen_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 決定処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool GameModeGuide::Decide() {
	if (!isOpen_) { return false; }

	if (Input::GetInstance()->IsTriggerButton(XInputButtons::BUTTON_A)) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 選択
///////////////////////////////////////////////////////////////////////////////////////////////

bool GameModeGuide::Select() {
	// 選択する
	bool isSelect = false;
	if (Input::GetInstance()->IsTriggerButton(XInputButtons::DPAD_UP) || Input::GetInstance()->GetLeftJoyStick(.5f).y > 0.0f) {
		selectModeIndex_++;
		isSelect = true;
	}

	if (Input::GetInstance()->IsTriggerButton(XInputButtons::DPAD_DOWN) || Input::GetInstance()->GetLeftJoyStick(.5f).y < 0.0f) {
		selectModeIndex_--;
		isSelect = true;
	}

	// clamp
	if (selectModeIndex_ < 0) {
		selectModeIndex_ = static_cast<int>(SelectModeType::kMax) - 1;
	} else if (selectModeIndex_ >= static_cast<int>(SelectModeType::kMax)) {
		selectModeIndex_ = 0;
	}

	selectModeType_ = static_cast<SelectModeType>(selectModeIndex_);

	// UIを揃える
	if (selectModeType_ == SelectModeType::ToGame) {
		select_->SetTranslate(goGame_->GetTranslate());
	} else if (selectModeType_ == SelectModeType::ToTutorial) {
		select_->SetTranslate(goTutorial_->GetTranslate());
	}

	return isSelect;
}
