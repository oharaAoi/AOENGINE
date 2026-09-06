#include "TutorialScene.h"

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

	UpdateContext();
	stepController_.Init(context_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialScene::Update() {

	const float deltaTime = GameTimer::DeltaTime();

	// リトライやクリア判定は挟まず、アクターだけを進める
	UpdateActors(deltaTime);

	UpdateContext();
	stepController_.Update(context_, deltaTime);

	// 最後のページまで終わったらタイトルへ戻る
	if (stepController_.IsAllFinished()) {
		nextSceneType_ = SceneType::Title;
		return;
	}

#ifdef _DEVELOPMENT
	ImGui::Begin("Tutorial");
	stepController_.Debug_Gui(context_);
	ImGui::End();
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ページへ渡す情報の更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialScene::UpdateContext() {

	// 次のページへ送る入力。XボタンとF キーの両方で送れる
	const bool isNextTriggered =
		Input::IsTriggerKey(kNextKey) || Input::IsTriggerButton(ButtonX);
	context_.SetNextTriggered(isNextTriggered);

	// コントローラーが繋がっていなければ、説明文をキーボード操作の方へ切り替える
	context_.SetPadConnected(Input::IsControllerConnected());
}
