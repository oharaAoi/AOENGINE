#include "AoiFramework.h"
#include "Engine/Engine.h"
#include "Engine/System/Manager/AssetsManager.h"
#include "Engine/System/Manager/MeshManager.h"
#include "Engine/System/Manager/ModelManager.h"


//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　終了処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AoiFramework::Finalize() {
	MeshManager::GetInstance()->Finalize();
	ModelManager::GetInstance()->Finalize();
	Engine::Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

AoiFramework::AoiFramework(uint32_t _kWindowWidth, uint32_t kWindowHeight, const char* _windowTitle) {
	windowWidth_ = _kWindowWidth;
	windowHeight_ = kWindowHeight;
	windowTitle_ = _windowTitle;
}

void AoiFramework::Init() {
	endRequest_ = false;
	logger_.Init();

	Engine::Initialize(windowWidth_, windowHeight_, windowTitle_);
	AssetsManager::GetInstance()->Init();

	logger_.CommentLog("Game Start!");
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　ゲーム処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AoiFramework::Run() {
	// 初期化
	Init();

	// ゲームループ
	while (Engine::ProcessMessage()) {

		// frame更新
		BeginFrame();

		// 更新
		Update();

		// 描画
		Draw();

		// frame終了
		EndFrame();

		// 終了リクエスト
		if (isEndRequest()) {
			break;
		}
	}

	// 解放処理
	Finalize();
}
