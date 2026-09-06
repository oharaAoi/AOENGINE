#include "TutorialScene.h"

#include <array>

#include "Engine/Lib/GameTimer.h"
#include "Engine/System/Input/Input.h"
#include "Engine/System/Manager/ImGuiManager.h"

#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Camera/FollowCamera.h"

using namespace AOENGINE;

namespace {
	// 次のページへ送るキー
	constexpr uint8_t kNextKey = DIK_F;
	// 前のページへ戻るキー
	constexpr uint8_t kBackKey = DIK_Q;
}

TutorialScene::~TutorialScene() {
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// スタート時に呼ばれる処理(パラメータの読み込み、設定などはココで行う)
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialScene::OnPlayStart() {

	// Player・ステージ・カメラの用意は GameScene と同じ
	GameScene::OnPlayStart();

	// ページが触るものを詰めてから、最初のページを始める
	context_.SetPlayer(player_.get());
	context_.SetBoss(boss_.get());
	context_.SetBlockField(&stageBlockField_);
	context_.SetCamera(followCamera_.get());

	isChangingPage_ = false;
	isGoingBack_ = false;

	// 説明文を読み込んでからテキストボックスを開く
	textTable_.Load();
	textBox_.Init();
	textBox_.Open();

	UpdateContext();
	stepController_.Init(context_);
	UpdateTextBox(0.0f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialScene::Update() {

	const float deltaTime = GameTimer::DeltaTime();

	// リトライやクリア判定は挟まず、アクターだけを進める
	UpdateActors(deltaTime);

	UpdateContext();

	if (isChangingPage_) {
		// 閉じきってからページを切り替え、また開く
		if (textBox_.IsClosed()) {
			isChangingPage_ = false;

			if (isGoingBack_) {
				stepController_.Back(context_);
			} else {
				stepController_.Advance(context_);
			}

			if (!stepController_.IsAllFinished()) {
				textBox_.Open();
			}
		}
	} else {
		stepController_.Update(context_, deltaTime);

		// 送る/戻す条件が揃ったら、まず閉じるところから始める
		if (stepController_.WantsAdvance(context_)) {
			isChangingPage_ = true;
			isGoingBack_ = false;
			textBox_.Close();
		} else if (stepController_.WantsBack(context_)) {
			isChangingPage_ = true;
			isGoingBack_ = true;
			textBox_.Close();
		}
	}

	UpdateTextBox(deltaTime);

	// 最後のページまで終わって、閉じきったらタイトルへ戻る
	if (stepController_.IsAllFinished() && textBox_.IsClosed()) {
		nextSceneType_ = SceneType::Title;
		return;
	}

#ifdef _DEVELOPMENT
	ImGui::Begin("Tutorial");
	stepController_.Debug_Gui(context_);
	if (ImGui::CollapsingHeader("UI")) {
		ImGui::PushID("TutorialUI");
		textBox_.Debug_Gui();
		ImGui::PopID();
	}
	ImGui::End();
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// テキストボックスの更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialScene::UpdateTextBox(float deltaTime) {

	const bool isPadConnected = context_.IsPadConnected();

	TutorialTextBoxUI::Content content{};

	// コントローラーの有無で、パッド用とキーボード用の説明が切り替わる。
	content.body = &textTable_.GetStepText(stepController_.GetCurrentTextKey(), isPadConnected);
	content.nextText = &textTable_.GetNextGuide(isPadConnected);
	content.backText = &textTable_.GetBackGuide(isPadConnected);

	// 打ち上げのように条件で進むページでは次への案内を、最初のページでは戻るの案内を出さない
	content.showNext = stepController_.CanSkipByInput();
	content.showBack = stepController_.CanBack();

	// ページが持っているぶんだけチェックの状態を集める
	const std::size_t checkCount = stepController_.GetCurrentCheckCount();
	for (std::size_t i = 0; i < content.checkShown.size() && i < checkCount; ++i) {
		content.checkShown[i] = stepController_.IsCurrentCleared(i);
	}
	content.pageIndex = stepController_.GetCurrentIndex();
	content.isPadConnected = isPadConnected;

	// 操作の説明はボタンの絵でも出す。こちらもパッドとキーボードで中身が変わる
	content.buttons = &textTable_.GetStepButtons(stepController_.GetCurrentTextKey(), isPadConnected);

	// 案内に添えるボタンは1つだけ使う
	const std::vector<std::string>& nextButtons = textTable_.GetNextGuideButtons(isPadConnected);
	if (!nextButtons.empty()) {
		content.nextButton = &nextButtons.front();
	}

	const std::vector<std::string>& backButtons = textTable_.GetBackGuideButtons(isPadConnected);
	if (!backButtons.empty()) {
		content.backButton = &backButtons.front();
	}

	textBox_.Update(deltaTime, content);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ページへ渡す情報の更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialScene::UpdateContext() {

	// 次のページへ送る入力。XボタンとF キーの両方で送れる
	const bool isNextTriggered =
		Input::IsTriggerKey(kNextKey) || Input::IsTriggerButton(ButtonX);
	context_.SetNextTriggered(isNextTriggered);

	// 前のページへ戻る入力
	const bool isBackTriggered =
		Input::IsTriggerKey(kBackKey) || Input::IsTriggerButton(ButtonY);
	context_.SetBackTriggered(isBackTriggered);

	// コントローラーが繋がっていなければ、説明文をキーボード操作の方へ切り替える
	context_.SetPadConnected(Input::IsControllerConnected());
}
