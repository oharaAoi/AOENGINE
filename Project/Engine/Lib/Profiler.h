#pragma once
#include <chrono>
#include <string>
#include <queue>
#include <utility>

namespace AOENGINE {

/// <summary>
/// 処理時間計測器
/// </summary>
class Profiler {
public:	// コンストラクタ

	Profiler() = default;
	~Profiler() = default;

public:

	/// <summary>
	/// 開始
	/// </summary>
	void Start();

	/// <summary>
	/// 終了
	/// </summary>
	void End(const std::string& _name);

	/// <summary>
	/// 表示
	/// </summary>
	void DisPlay();

public:

	std::string name_;
	std::chrono::high_resolution_clock::time_point start_;

	std::queue<std::pair<std::string, double>> timeQueue_;

};

}