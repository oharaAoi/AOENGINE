#pragma once
#include <cstdint>
#include <chrono>
#include <thread>


/// <summary>
/// フレームレートを固定するためのクラス
/// </summary>
class GameTimer {
public:

	GameTimer(const uint32_t& fps);
	~GameTimer();

	void CalculationFrame();

	static float DeltaTime() { return deletaTime_; }
	static float TotalTime(){ return totalTime_; }
	static float TimeRate() { return timeRate_; }

	static float fps_;

	static void SetTimeScale(float _time) { timeScale_ = _time; }

private:
	std::chrono::steady_clock::time_point preFrameTime_;	// 前フレームの時間
	std::chrono::milliseconds frameDuration_;				// フレームの間隔
	static float deletaTime_;
	static float timeRate_;
	static float totalTime_;

	static float timeScale_;
};

const float kDeltaTime_ = (1.0f / GameTimer::fps_);