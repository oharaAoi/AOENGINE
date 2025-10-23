#pragma once

/// <summary>
/// 時間をカウントしたりする際に使用する構造体
/// </summary>
struct Timer {
	Timer(float _targetTime = 1.f) : targetTime_(_targetTime) { timer_ = 0; }

	/// <summary>
	/// 実行
	/// </summary>
	/// <param name="_deltaTime"></param>
	/// <returns></returns>
	bool Run(float _deltaTime);

	float timer_ = 0;
	float targetTime_ = 0;

	float t_ = 0;
};

