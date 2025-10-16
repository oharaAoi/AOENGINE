#pragma once

/// <summary>
/// 時間をカウントしたりする際に使用するクラス
/// </summary>
struct Timer {
	Timer(float _targetTime = 1.f) : targetTime_(_targetTime) { timer_ = 0; }

	bool Run(float _deltaTime);

	float timer_ = 0;
	float targetTime_ = 0;

	float t_ = 0;
};

