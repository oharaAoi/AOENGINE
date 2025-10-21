#include "GameTimer.h"

float GameTimer::deltaTime_ = 0.0f;
float GameTimer::fps_ = 60.0f;
float GameTimer::timeRate_ = 0.0f;
float GameTimer::totalTime_ = 0.0f;
float GameTimer::timeScale_ = 1.0f;

GameTimer::GameTimer(const uint32_t& fps) {
	frameDuration_ = std::chrono::milliseconds(1000 / fps);
	preFrameTime_ = std::chrono::steady_clock::now();
    deltaTime_ = 1.0f / static_cast<float>(fps);
}

GameTimer::~GameTimer() {
}

void GameTimer::CalculationFrame() {
    using clock = std::chrono::steady_clock;

    auto currentTime = clock::now();
    float duration = std::chrono::duration<float>(currentTime - preFrameTime_).count();

    // フレーム間隔の上限を設定 (例: 10fps分)
    constexpr float MAX_DELTA = 0.1f;
    if (duration > MAX_DELTA) {
        duration = MAX_DELTA;
    }

    deltaTime_ = duration * timeScale_;
    timeRate_ = deltaTime_ / kDeltaTime_;
    totalTime_ += deltaTime_;

    preFrameTime_ = currentTime;
}
