#include "GameClearUI.h"

// engine
#include <Engine/Utilities/SceneObjectFinder.h>
#include <Engine/Module/Components/2d/Text.h>
#include <Engine/Module/Components/2d/Sprite.h>
#include <Engine/Lib/GameTimer.h>
#include <Engine/System/Input/Input.h>

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameClearUI::Init() {
	selectIndex_ = 0;
	coolTimer_ = AOENGINE::Timer(0.2f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

GameClearUI::GameClearItem GameClearUI::Update() {
	// 次の行動の選択
	if (!coolTimer_.Run(AOENGINE::GameTimer::FixedDeltaTime())) {
		SelectItem();
	}

	GameClearUI::GameClearItem current = CurrentSelect();
	if (current == GameClearUI::GameClearItem::Retry) {
		AOENGINE::Text* retryText = FindSceneObject<AOENGINE::Text>("Retry");
		AOENGINE::Text* exitText = FindSceneObject<AOENGINE::Text>("Exit");
		retryText->SetTextColor(Colors::Linear::red);
		exitText->SetTextColor(Colors::Linear::white);
	} else if (current == GameClearUI::GameClearItem::Exit) {
		AOENGINE::Text* retryText = FindSceneObject<AOENGINE::Text>("Retry");
		AOENGINE::Text* exitText = FindSceneObject<AOENGINE::Text>("Exit");
		retryText->SetTextColor(Colors::Linear::white);
		exitText->SetTextColor(Colors::Linear::red);
	}

	// 決定を行う
	if (DecisionItem()) {
		return current;
	}

	return GameClearItem::Pause;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 次の項目を選択する
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameClearUI::SelectItem() {
	constexpr int kMaxItem = 2;
	AOENGINE::Input* input = AOENGINE::Input::GetInstance();

	auto press_down = [&]() {
		selectIndex_ = (selectIndex_ + 1) % kMaxItem;
		coolTimer_.Reset();
		};

	// 上キーが押された時の擬似処理
	auto press_up = [&]() {
		// 0未満になった時に正しく最大値に戻すための計算
		selectIndex_ = (selectIndex_ - 1 + kMaxItem) % kMaxItem;
		coolTimer_.Reset();
		};

	// キー入力判定
	if (input->GetKey(DIK_W) || input->GetKey(DIK_UP)) { press_up(); }
	if (input->GetKey(DIK_S) || input->GetKey(DIK_DOWN)) { press_down(); }

	// stick判定
	if (input->GetLeftJoyStick().y >= 0.2f) {
		press_down();
	} else if (input->GetLeftJoyStick().y <= -0.2f) {
		press_up();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 次の項目の決定
//////////////////////////////////////////////////////////////////////////////////////////////////

bool GameClearUI::DecisionItem() {
	AOENGINE::Input* input = AOENGINE::Input::GetInstance();
	return input->GetKey(DIK_SPACE) || input->IsTriggerButton(XInputButtons::ButtonA);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 現在選択中の項目を返す
//////////////////////////////////////////////////////////////////////////////////////////////////

GameClearUI::GameClearItem GameClearUI::CurrentSelect() {
	if (selectIndex_ == 0) {
		return GameClearUI::GameClearItem::Retry;
	} else if (selectIndex_ == 1) {
		return GameClearUI::GameClearItem::Exit;
	}

	return GameClearUI::GameClearItem::Pause;
}
