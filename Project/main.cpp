#include <Windows.h>
#include "Game/RunTime/GameCore.h"
#include "Engine/Core/AoiFramework.h"

static const uint32_t kWindowWidth_ = 1280;
static const uint32_t kWindowHeight_ = 720;

static const char kTitleBar[] = "4005_メテオギュット";

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// ウィンドウ生成前に設定し、OSによる座標のDPI仮想化を無効にする。
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	
	// 出力ウィンドウへの文字出力
	D3DResourceLeakChecker debugChecker;

	std::unique_ptr<AOENGINE::AoiFramework> game = std::make_unique<GameCore>(kWindowWidth_, kWindowHeight_, kTitleBar);
	game->Run();

	return 0;
}
