#pragma once

/// stl
#include <vector>

/// engine
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Quaternion.h"

/// game
#include "Game/Path/LineLoader.h"

/// <summary>
/// LineLoaderで読み込んだpoints上を移動するクラス。
/// 全セグメントの距離を足した総距離を持ち、その上を1次元の進捗として
/// accel / velocity / maxVelocity で進める。座標と進行方向を向く回転を返すだけで、
/// WorldTransformへの反映は呼び出し側が行う
/// </summary>
class LinePathMover{
public:

	/// <summary>終点まで進んだ時の挙動</summary>
	enum class EndAction{
		Stop,		// 終点で止まる
		Loop,		// 始点へ戻って繰り返す
		PingPong,	// 折り返して逆向きに進む
	};

	/// <summary>調整値</summary>
	struct Params{
		float accel = 0.0f;			// 1秒あたりの加速度
		float maxVelocity = 0.0f;	// 速さの上限。これ以上は加速しない
		float initVelocity = 0.0f;	// Reset時の初速
		float rotateLerpRate = 0.0f;// 進行方向を向く速さ。0以下なら即座に向く
	};

	LinePathMover() = default;
	~LinePathMover() = default;

	/// <summary>
	/// 移動するpointsを設定する。総距離もここで求める
	/// </summary>
	/// <param name="points">: Lineのサンプリング座標</param>
	/// <param name="isClosed">: 閉じたLineか(終点から始点へ戻るセグメントを繋ぐ)</param>
	void SetPoints(const std::vector<Math::Vector3>& points, bool isClosed = false);

	/// <summary>
	/// 読み込んだLineをそのまま設定する
	/// </summary>
	/// <param name="lineData">: LineLoaderが読み込んだLine</param>
	void SetLineData(const LineLoader::LineData& lineData);

	/// <summary>
	/// 進捗を始点に戻し、速さを初速に戻す
	/// </summary>
	void Reset();

	/// <summary>
	/// 加速度で速さを更新し、その分だけ進捗を進める
	/// </summary>
	/// <param name="deltaTime">: 経過時間</param>
	void Update(float deltaTime);

public:		// setter

	void SetParams(const Params& params){ params_ = params; }

	void SetEndAction(EndAction endAction){ endAction_ = endAction; }

	/// <summary>
	/// 進捗を直接指定する(0 ～ 総距離)
	/// </summary>
	void SetDistance(float distance);

	/// <summary>
	/// 現在の速さを直接指定する
	/// </summary>
	void SetVelocity(float velocity);

	/// <summary>
	/// 進む向きを反転させる
	/// </summary>
	void SetReverse(bool isReverse);

public:		// getter

	/// 現在の座標
	const Math::Vector3& GetPosition() const{ return position_; }
	/// 進行方向を向く回転
	const Math::Quaternion& GetRotate() const{ return rotate_; }
	/// 進行方向(正規化済み)
	const Math::Vector3& GetDirection() const{ return direction_; }

	/// 現在の速さ
	float GetVelocity() const{ return velocity_; }
	/// 始点から進んだ距離(1次元の進捗)
	float GetDistance() const{ return distance_; }
	/// 全セグメントの距離を足した総距離
	float GetTotalLength() const{ return totalLength_; }
	/// 進捗の割合(0 ～ 1)
	float GetProgress() const;

	/// 終点まで進んで停止したか(EndAction::Stopのみ)
	bool IsFinished() const{ return isFinished_; }
	/// 移動できるpointsを持っているか
	bool IsValid() const;

	const std::vector<Math::Vector3>& GetPoints() const{ return points_; }

	/// <summary>
	/// 始点から指定した距離だけ進んだ座標を取得する
	/// </summary>
	/// <param name="distance">: 始点からの距離</param>
	Math::Vector3 GetPointAtDistance(float distance) const;

	/// <summary>
	/// 始点から指定した距離の位置での進行方向を取得する
	/// </summary>
	/// <param name="distance">: 始点からの距離</param>
	Math::Vector3 GetDirectionAtDistance(float distance) const;

private:

	/// <summary>
	/// 全セグメントの距離を足して総距離と各セグメントまでの累積距離を作る
	/// </summary>
	void BuildLengths();

	/// <summary>
	/// 終端まで進んだ進捗を EndAction に従って収める
	/// </summary>
	void ResolveDistance();

	/// <summary>
	/// 現在の進捗から座標と進行方向を求める
	/// </summary>
	void UpdateTransform(float deltaTime, bool isSnapRotate);

	/// <summary>
	/// 指定した距離がどのセグメント上にあるかを求める
	/// </summary>
	/// <param name="distance">: 始点からの距離</param>
	/// <param name="rate">: そのセグメント内での割合(0 ～ 1)</param>
	/// <returns>セグメントの番号</returns>
	size_t FindSegment(float distance, float& rate) const;

	/// セグメントの数。閉じている場合は終点から始点へ戻る分を含む
	size_t GetSegmentCount() const;

private:

	/// Lineのサンプリング座標
	std::vector<Math::Vector3> points_;
	/// 各セグメントの手前までの累積距離。要素数は セグメント数 + 1
	std::vector<float> accumulatedLengths_;
	/// 全セグメントの距離の合計
	float totalLength_ = 0.0f;
	/// 閉じたLineか
	bool isClosed_ = false;

	Params params_{};
	EndAction endAction_ = EndAction::Stop;

	/// 始点から進んだ距離
	float distance_ = 0.0f;
	/// 現在の速さ(常に0以上。進む向きは moveSign_ が持つ)
	float velocity_ = 0.0f;
	/// 進む向き。+1で始点から終点、-1で逆向き
	float moveSign_ = 1.0f;
	/// 終点まで進んで停止したか
	bool isFinished_ = false;

	Math::Vector3 position_{};
	Math::Vector3 direction_ = CVector3::FORWARD;
	Math::Quaternion rotate_{};

};
