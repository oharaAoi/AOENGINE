#pragma once
#include <string>
#include <vector>

#include "Engine/Lib/Math/Vector3.h"

/// <summary>
/// プレイヤーがブロックグループを接続し、集合・打ち上げを要求するまでの状態を保持するクラス
/// このクラスはグループIDと接続した位置だけを扱い、ブロックの実体やステージ側の派生データは一切持たない
/// </summary>
class BlockGroupConnectState {
public:

	// 状態
	enum class Phase {
		Idle,		// 何もしていない
		Connecting,	// 接続受付中
		Gathering,	// 集合中(打ち上げ待ち)
	};

	// 調整値
	struct Params {
		float connectableTime;	// 接続を受け付ける時間
		float launchWaitTime;	// 集合を始めてから自動で打ち上げるまでの時間
	};

	// 毎フレーム外から渡す情報
	struct Context {
		Math::Vector3 playerPosition;	// プレイヤーの現在位置
		bool isGrounded = false;		// 足場に乗っているか
		bool launchTriggered = false;	// 打ち上げ入力が押されたか
	};

	// 接続されたグループ1つ分の情報
	struct ConnectedGroup {
		int groupId = -1;					// 接続されたグループID
		Math::Vector3 connectPosition{};	// 接続した時のプレイヤーの位置
	};

	BlockGroupConnectState() = default;
	~BlockGroupConnectState() = default;

	/// <summary>
	/// 接続受付を開始する。Idle以外(集合中など)では何もしない
	/// </summary>
	void Begin();

	/// <summary>
	/// 更新。接続受付中は残り時間を、集合中は打ち上げまでの残り時間を進める
	/// </summary>
	void Update(float deltaTime, const Context& context, const Params& params);

	/// <summary>
	/// 接続されたグループIDを追加する
	/// </summary>
	/// <param name="groupId">追加するグループID</param>
	/// <returns>追加できた場合はtrue</returns>
	bool TryAdd(int groupId);

	/// <summary>
	/// 状態をリセットする
	/// </summary>
	void Clear();

	// 状態名の取得(デバッグ表示用)
	const std::string& GetPhaseName() const;

private:

	/// <summary>接続受付中の更新。受付時間が尽きたら集合へ移る</summary>
	void UpdateConnecting(float deltaTime, const Context& context);
	/// <summary>集合中の更新。専用タイマーが尽きるか入力があれば打ち上げを要求する</summary>
	void UpdateGathering(float deltaTime, const Context& context);
	/// <summary>集合フェーズへ移る。集合地点と打ち上げまでの残り時間を確定させる</summary>
	void BeginGather(const Context& context);

private:

	Phase phase_ = Phase::Idle;

	float remainingTime_ = 0.0f;	// 接続受付の残り時間
	float launchTimer_ = 0.0f;		// 集合開始から打ち上げまでの残り時間

	std::vector<ConnectedGroup> connectedGroups_;	// 接続されたグループを接続順に保持する

	Math::Vector3 gatherPoint_{};			// 集合地点(プレイヤーが最後に乗っていた場所)
	Math::Vector3 lastPlayerPosition_{};	// 直近のUpdateで受け取ったプレイヤーの位置
	Math::Vector3 lastGroundedPosition_{};	// 最後に足場へ乗っていた時のプレイヤーの位置
	bool hasGroundedPosition_ = false;		// lastGroundedPosition_ が有効か

	bool gatherStarted_ = false;	// そのフレームに集合が始まったか
	bool launchRequested_ = false;	// そのフレームに打ち上げが要求されたか

	Params params_{};

public: // accessor

	// 接続受付中かどうか
	bool CanConnect() const { return phase_ == Phase::Connecting && remainingTime_ > 0.0f; }
	// 現在の状態
	Phase GetPhase() const { return phase_; }
	// 接続されたグループのリストを取得
	const std::vector<ConnectedGroup>& GetConnectedGroups() const { return connectedGroups_; }
	// 集合地点を取得
	const Math::Vector3& GetGatherPoint() const { return gatherPoint_; }
	// 接続受付の残り時間を取得
	float GetRemainingTime() const { return remainingTime_; }
	// 打ち上げまでの残り時間を取得
	float GetLaunchTimer() const { return launchTimer_; }
	// そのフレームに集合が始まったか
	bool IsGatherStarted() const { return gatherStarted_; }
	// そのフレームに打ち上げが要求されたか
	bool IsLaunchRequested() const { return launchRequested_; }

};
