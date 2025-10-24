#include <Windows.h>
#include "Game/RunTime/GameCore.h"
#include "Engine/Core/AoiFramework.h"

static const uint32_t kWindowWidth_ = 1280;
static const uint32_t kWindowHeight_ = 720;

static const char kTitleBar[] = "LE3A_06_オオハラ_アオイ";

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// 出力ウィンドウへの文字出力
	D3DResourceLeakChecker debugChecker;

	std::unique_ptr<AoiFramework> game = std::make_unique<GameCore>(kWindowWidth_, kWindowHeight_, kTitleBar);
	game->Run();

	return 0;
}