#include "RetryUI.h"

// engine
#include <Engine/Utilities/SceneObjectFinder.h>
#include <Engine/Module/Components/2d/Text.h>
#include <Engine/Module/Components/2d/Sprite.h>
#include <Engine/Lib/GameTimer.h>
#include <Engine/System/Input/Input.h>
#include <Engine/Core/Engine.h>

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
	// 選択を受け付ける間隔
	const float kSelectCoolTime = 0.2f;
	// スティックの間隔。1回倒しただけで何個も進まないよう、キーより長くする
	const float kStickCoolTime = 0.28f;
}

void RetryUI::Init() {
	selectIndex_ = 0;
	coolTimer_ = AOENGINE::Timer(kSelectCoolTime);
	stickCoolTimer_ = AOENGINE::Timer(kStickCoolTime);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////

RetryItem RetryUI::Update(bool isPlayerAlive) {
	if (!isPlayerAlive) {
		// retryUI達の有効化
		AOENGINE::Sprite* retry = FindSceneObject<AOENGINE::Sprite>("Retry");

		if (!retry) {
			// もしRetryが見つからなかった場合は、Titleを返す
			return RetryItem::Title;
		}

		retry->SetActive(true);

		// 次の行動の選択。スティックはキーより長く待たせる
		const float deltaTime = AOENGINE::GameTimer::FixedDeltaTime();
		const bool canUseStick = !stickCoolTimer_.Run(deltaTime);
		if (!coolTimer_.Run(deltaTime)) {
			SelectItem(canUseStick);
		}

		RetryItem current = CurrentSelect();
		if (current == RetryItem::Retry) {
			AOENGINE::Text* retryText = FindSceneObject<AOENGINE::Text>("Text_Retry");
			AOENGINE::Text* titleText = FindSceneObject<AOENGINE::Text>("Text_Title");
			retryText->SetTextColor(Colors::Linear::red);
			titleText->SetTextColor(Colors::Linear::white);
		} else if (current == RetryItem::Title) {
			AOENGINE::Text* retryText = FindSceneObject<AOENGINE::Text>("Text_Retry");
			AOENGINE::Text* titleText = FindSceneObject<AOENGINE::Text>("Text_Title");
			retryText->SetTextColor(Colors::Linear::white);
			titleText->SetTextColor(Colors::Linear::red);
		}

		// 決定を行う
		if (DecisionItem()) {
			Engine::GetSoundManager()->Play("Decided");
			return current;
		}
	} else {
		selectIndex_ = 0;
		coolTimer_ = AOENGINE::Timer(kSelectCoolTime);
		stickCoolTimer_ = AOENGINE::Timer(kStickCoolTime);
	}

	return RetryItem::Pause;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 次の項目を選択する
//////////////////////////////////////////////////////////////////////////////////////////////////

void RetryUI::SelectItem(bool canUseStick) {
	constexpr int kMaxItem = 2;
	AOENGINE::Input* input = AOENGINE::Input::GetInstance();

	auto press_down = [&]() {
		selectIndex_ = (selectIndex_ + 1) % kMaxItem;
		coolTimer_.Reset();
		stickCoolTimer_.Reset();
		Engine::GetSoundManager()->Play("Select");
		};

	// 上キーが押された時の擬似処理
	auto press_up = [&]() {
		// 0未満になった時に正しく最大値に戻すための計算
		selectIndex_ = (selectIndex_ - 1 + kMaxItem) % kMaxItem;
		coolTimer_.Reset();
		stickCoolTimer_.Reset();
		Engine::GetSoundManager()->Play("Select");
		};

	// キー入力判定
	if (input->GetKey(DIK_W) || input->GetKey(DIK_UP)) { press_up(); }
	if (input->GetKey(DIK_S) || input->GetKey(DIK_DOWN)) { press_down(); }

	// 十字キー判定
	if (input->IsPressButton(DpadUp)) { press_up(); }
	if (input->IsPressButton(DpadDown)) { press_down(); }

	// stick判定
	if (!canUseStick) {
		return;
	}
	if (input->GetLeftJoyStick().y >= 0.2f) {
		press_down();
	} else if (input->GetLeftJoyStick().y <= -0.2f) {
		press_up();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 次の項目の決定
//////////////////////////////////////////////////////////////////////////////////////////////////

bool RetryUI::DecisionItem() {
	AOENGINE::Input* input = AOENGINE::Input::GetInstance();
	return input->GetKey(DIK_SPACE) || input->IsTriggerButton(XInputButtons::ButtonA);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 現在選択中の項目を返す
//////////////////////////////////////////////////////////////////////////////////////////////////

RetryItem RetryUI::CurrentSelect() {
	if (selectIndex_ == 0) {
		return RetryItem::Retry;
	} else if (selectIndex_ == 1) {
		return RetryItem::Title;
	}

	return RetryItem::Pause;
}

bool RetryUI::StartEffect() {
	AOENGINE::Sprite* retry = FindSceneObject<AOENGINE::Sprite>("Retry_title");
	AOENGINE::Sprite* goRetry = FindSceneObject<AOENGINE::Sprite>("Text_Retry");
	AOENGINE::Sprite* goTitle = FindSceneObject<AOENGINE::Sprite>("Text_Title");

	if (!retry || !goRetry || !goTitle) {
		return false;
	}

	if (isFall_) {
		Math::Vector2 pos = retry->GetTranslate();
		pos.y -= retryParametor_.diffLength;
		SetFall(pos, retry->GetTranslate(), 0);

		pos = goRetry->GetTranslate();
		pos.y -= retryParametor_.diffLength;
		SetFall(pos, goRetry->GetTranslate(), 1);

		pos = goTitle->GetTranslate();
		pos.y -= retryParametor_.diffLength;
		SetFall(pos, goTitle->GetTranslate(), 2);

		isFall_ = false;
	}

	if (fallTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		SpriteFall(retry, 0);
		SpriteFall(goRetry, 1);
		SpriteFall(goTitle, 2);
	} else {
		return true;
	}

	return false;
}

void RetryUI::SetFall(const Math::Vector2& startPos, const Math::Vector2& endPos, int index) {
	animationTween_[index].Init(startPos, endPos, retryParametor_.fallTime, static_cast<int>(EasingType::Out::Elastic), LoopType::Stop);
}

void RetryUI::SpriteFall(AOENGINE::Sprite* sprite, int index) {
	animationTween_[index].Update(AOENGINE::GameTimer::DeltaTime());
	sprite->SetTranslate(animationTween_[index].GetValue());
}
