#include "GameTimer.h"
#include "Engine/System/Manager/TextureManager.h"
#ifdef _DEBUG
#include "Externals/ImGui/imgui.h"
#include "Engine/System/Manager/ImGuiManager.h"
#endif

float GameTimer::deletaTime_ = 0.0f;
float GameTimer::fps_ = 60.0f;
float GameTimer::timeRate_ = 0.0f;
float GameTimer::totalTime_ = 0.0f;
float GameTimer::timeScale_ = 1.0f;

GameTimer::GameTimer(const uint32_t& fps) {
	frameDuration_ = std::chrono::milliseconds(1000 / fps);
	preFrameTime_ = std::chrono::steady_clock::now();
	deletaTime_ = 1.0f / static_cast<float>(fps);
}

GameTimer::~GameTimer() {
}

void GameTimer::CalculationFrame() {
	auto currentTime = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration<float>(currentTime - preFrameTime_).count();

	// deltaTimeが一定超えないように
	if (duration > 1.0f) {
		duration = kDeltaTime_; 
	}

	deletaTime_ = duration * timeScale_;
	timeRate_ = deletaTime_ / kDeltaTime_;
	totalTime_ += deletaTime_;

	preFrameTime_ = currentTime;
}
