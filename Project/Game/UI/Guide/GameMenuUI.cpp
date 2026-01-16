#include "GameMenuUI.h"
#include "Engine.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/System/Input/Input.h"
#include "Engine/System/Editor/Window/EditorWindows.h"

GameMenuUI::~GameMenuUI() {
	bg_->SetIsDestroy(true);
	goTitle_->SetIsDestroy(true);
	goGame_->SetIsDestroy(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void GameMenuUI::Init() {
	param_.Load();

	AOENGINE::Canvas2d* canvas = Engine::GetCanvas2d();
	bg_ = canvas->AddSprite("white.png", "menuBg");
	goTitle_ = canvas->AddSprite("goTitle.png", "goTitle");
	goGame_ = canvas->AddSprite("goBackGame.png", "goBackGame");

	bg_->Load("GameMenu", "bg");
	goTitle_->Load("GameMenu", "goTitle");
	goGame_->Load("GameMenu", "goGame");

	goTitle_->SetColor(Colors::Linear::white);
	goGame_->SetColor(param_.noSelectColor);

	AddChild(bg_);
	AddChild(goTitle_);
	AddChild(goGame_);

	item_ = GameMenuSelectItem::Go_Title;
	timer_.targetTime_ = param_.inputCoolTime;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void GameMenuUI::Update() {
	Select();

	if (item_ == GameMenuSelectItem::Go_Title) {
		goTitle_->SetColor(Colors::Linear::white);
		goGame_->SetColor(param_.noSelectColor);
	} else {
		goTitle_->SetColor(param_.noSelectColor);
		goGame_->SetColor(Colors::Linear::white);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void GameMenuUI::Debug_Gui() {
	param_.Debug_Gui();
	timer_.targetTime_ = param_.inputCoolTime;
}

void GameMenuUI::Parameter::Debug_Gui() {
	ImGui::DragFloat("入力のクールタイム", &inputCoolTime, 0.1f);
	ImGui::ColorEdit4("選択されていない項目の色", &noSelectColor.r);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 選択
///////////////////////////////////////////////////////////////////////////////////////////////

void GameMenuUI::Select() {
	AOENGINE::Input* input = AOENGINE::Input::GetInstance();

	if (!timer_.Run(AOENGINE::GameTimer::FixedDeltaTime())) {
		// 入力判定を取る
		if (std::abs(input->GetLeftJoyStick().y) >= 0.1f) {
			ChangeItem();
		}

		if (input->IsTriggerButton(XInputButtons::DpadDown) ||
			input->IsTriggerButton(XInputButtons::DpadUp)) {
			ChangeItem();
		}
	}
}

void GameMenuUI::ChangeItem() {
	// 項目を次に進める
	uint32_t index = static_cast<uint32_t>(item_) + 1;
	if (index >= static_cast<uint32_t>(GameMenuSelectItem::kMax)) {
		index = 0;
	}
	item_ = static_cast<GameMenuSelectItem>(index);

	// 入力のクールタイム
	timer_.timer_ = 0;
}

GameMenuSelectItem GameMenuUI::Decide() {
	AOENGINE::Input* input = AOENGINE::Input::GetInstance();
	if (input->IsTriggerButton(XInputButtons::ButtonA)) {
		return item_;
	}
	return GameMenuSelectItem::kMax;
}