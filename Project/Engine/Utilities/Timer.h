#pragma once

namespace AOENGINE {

/// <summary>
/// 時間をカウントしたりする際に使用する構造体
/// </summary>
struct Timer {
	Timer(float _targetTime = 1.f) : targetTime_(_targetTime) { timer_ = 0; }

	/// <summary>
	/// 実行
	/// </summary>
	/// <param name="_deltaTime"></param>
	/// <returns>目標時間まで達していないならTrueを返す</returns>
	bool Run(float _deltaTime);

	float timer_ = 0;
	float targetTime_ = 0;

	float t_ = 0;

	bool isStop_ = false;
};

}