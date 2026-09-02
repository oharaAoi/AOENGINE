#pragma once
#include <vector>

/// <summary>
/// プレイヤーがブロックグループを接続している最中の状態を保持するクラス
/// このクラスはグループIDのみを扱い、ブロックの実体やステージ側の派生データは一切持たない
/// </summary>
class BlockGroupConnectState {
public:

	BlockGroupConnectState() = default;
	~BlockGroupConnectState() = default;

	/// <summary>
	/// 接続受付を開始する
	/// </summary>
	/// <param name="connectableTime">接続受付の残り時間として設定する値</param>
	void Begin(float connectableTime);

	/// <summary>
	/// 更新。remainingTime_ を減算する
	/// </summary>
	void Update(float deltaTime);

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

private:

	float remainingTime_ = 0.0f;	// 接続受付の残り時間
	std::vector<int> groupIds_;	// 接続されたグループIDを接続順に保持する

public: // accessor

	// 接続受付中かどうか
	bool CanConnect() const { return remainingTime_ > 0.0f; }
	// 接続されたグループIDのリストを取得
	const std::vector<int>& GetGroupIds() const { return groupIds_; }
	// 接続受付の残り時間を取得
	float GetRemainingTime() const { return remainingTime_; }

};
