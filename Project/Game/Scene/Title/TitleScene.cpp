#include "TitleScene.h"

#include "Engine/Core/Engine.h"

#include "Engine/Lib/GameTimer.h"
#include "Engine/System/Input/Input.h"
#include "Engine/System/Manager/ImGuiManager.h"

TitleScene::~TitleScene() {
}

void TitleScene::Finalize() {
	Engine::GetSoundManager()->Stop(bgmHandle_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化処理(インスタンスの宣言など)
//////////////////////////////////////////////////////////////////////////////////////////////////

void TitleScene::Init() {
	bgmHandle_ = Engine::GetSoundManager()->Play("TitleBGM");
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// スタート時に呼ばれる処理(パラメータの読み込み、設定などはココで行う)
//////////////////////////////////////////////////////////////////////////////////////////////////

void TitleScene::OnPlayStart() {
	titleUI_.Init();
	meteoEffect_.Init();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void TitleScene::Update() {

	// 背景の隕石を流す
	meteoEffect_.Update(AOENGINE::GameTimer::DeltaTime());

#ifdef _DEVELOPMENT
	ImGui::Begin("TitleScene Parameters");
	if (ImGui::CollapsingHeader("Meteo")) {
		ImGui::PushID("TitleMeteo");
		meteoEffect_.Debug_Gui();
		ImGui::PopID();
	}
	ImGui::End();
#endif

	// 次のシーンの選択
	TitleUI::TitleItem current = titleUI_.Update();
	if (current == TitleUI::TitleItem::Start) {
		nextSceneType_ = SceneType::Game;
	} else if (current == TitleUI::TitleItem::Tutorial) {
		nextSceneType_ = SceneType::Tutorial;
	} else if (current == TitleUI::TitleItem::Exit) {
		endRequest_ = true;
	}
}