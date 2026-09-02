#pragma once

/// stl
#include <vector>

/// engine
#include "Engine/Lib/Math/Vector3.h"

class Block;
class StageBlockField;

/// <summary>
/// 接続されたブロックグループを、接続した順に次のブロックへ最短距離で渡らせて1箇所に集め、
/// 上へ打ち上げるクラス。
/// ブロックの所有権は持たない(所有権は StageSegment 側にある)。
/// 打ち上げたグループは足場ではなくなるため、集合開始時に StageBlockField の表からは外す。
/// </summary>
class BlockGroupLauncher{
public:

	/// <summary>調整値</summary>
	struct Params{
		float gatherSpeed;		// 集合地点へ向かって移動する速さ
		float launchSpeed;		// 打ち上げの初速
		float launchAccel;		// 打ち上げ中の加速度
		float launchLifeTime;	// 打ち上げてから制御を手放すまでの時間
	};

	/// <summary>集合させるグループ1つ分の指定</summary>
	struct Target{
		int groupId = -1;					// 集合させるグループID
		Math::Vector3 connectPosition{};	// そのグループへ接続した時のプレイヤーの位置
	};

	/// <summary>集合の依頼内容</summary>
	struct GatherRequest{
		std::vector<Target> targets;	// 接続順のグループ
		Math::Vector3 gatherPoint{};	// 集合地点
	};

	/// <summary>状態</summary>
	enum class State{
		Idle,		// 何もしていない
		Gathering,	// 集合中
		Launched,	// 打ち上げ中
	};

	BlockGroupLauncher() = default;
	~BlockGroupLauncher() = default;

	/// <summary>
	/// 集合を開始する。指定されたグループを StageBlockField から引き当てて経路を組み立てる
	/// </summary>
	/// <param name="request">集合の依頼内容</param>
	/// <param name="params">調整値</param>
	void BeginGather(const GatherRequest& request,const Params& params);

	/// <summary>
	/// 集めたブロックを上へ打ち上げる
	/// </summary>
	void Launch();

	/// <summary>
	/// 更新。集合中は経路上を、打ち上げ中は上方向へブロックを動かす
	/// </summary>
	void Update(float deltaTime);

	/// <summary>
	/// 制御を手放す。ブロックの速度はそのまま残す(飛んで行った先はもう管理しない)
	/// </summary>
	void Clear();

private:

	/// <summary>集合中のグループ1つ分の作業データ</summary>
	struct GatheringGroup{
		std::vector<Block*> blocks;			// グループのブロック(非所有)
		std::vector<Math::Vector3> offsets;	// 接続した時点のプレイヤー位置から見た各ブロックの相対位置
		std::vector<Math::Vector3> path;	// 次のブロック -> ... -> 集合地点 と直線で結んだ経路
		float pathLength = 0.0f;			// 経路の全長
		float progress = 0.0f;				// 経路上を進んだ距離
	};

	/// <summary>request の targetIndex 番目のグループから作業データを組み立てる</summary>
	bool MakeGatheringGroup(const GatherRequest& request,size_t targetIndex,GatheringGroup& outGroup) const;
	/// <summary>集合中の更新。経路上の目標位置へブロックを動かす</summary>
	void UpdateGathering(float deltaTime);
	/// <summary>打ち上げ中の更新。上方向へブロックを動かす</summary>
	void UpdateLaunched(float deltaTime);
	/// <summary>経路上の位置に合わせてグループのブロックを動かす</summary>
	void MoveGroup(const GatheringGroup& group,const Math::Vector3& pathPoint,float deltaTime) const;
	/// <summary>グループの全ブロックへ同じ速度を設定する</summary>
	void SetGroupVelocity(const GatheringGroup& group,const Math::Vector3& velocity,float deltaTime) const;

	/// <summary>経路上を distance だけ進んだ位置を求める</summary>
	static Math::Vector3 SamplePath(const std::vector<Math::Vector3>& path,float distance);
	/// <summary>経路の全長を求める</summary>
	static float ComputePathLength(const std::vector<Math::Vector3>& path);
	/// <summary>ブロックのワールド座標を取得する</summary>
	static Math::Vector3 GetBlockPosition(const Block* block);
	/// <summary>ブロックを diff だけ動かす。Rigidbodyがあれば速度で、無ければ座標を直接動かす</summary>
	static void MoveBlock(Block* block,const Math::Vector3& diff,float deltaTime);

	State state_ = State::Idle;

	std::vector<GatheringGroup> groups_;	// 集合・打ち上げの対象

	float launchVelocityY_ = 0.0f;	// 現在の打ち上げ速度
	float launchTimer_ = 0.0f;		// 制御を手放すまでの残り時間

	Params params_{};

	StageBlockField* pField_ = nullptr;	// 非所有

public: // accessor

	void SetField(StageBlockField* field){ pField_ = field; }

	State GetState() const{ return state_; }
	bool IsActive() const{ return state_ != State::Idle; }
	// 集合・打ち上げの対象になっているグループ数
	int GetGroupCount() const{ return static_cast<int>(groups_.size()); }

};
