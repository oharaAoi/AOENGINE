#pragma once

/// stl
#include <memory>
#include <vector>

/// engine
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Module/Components/WorldTransform.h"

/// game
#include "Game/Effect/EffectObjectGroup.h"

class Block;
class StageBlockField;

namespace AOENGINE{
	class Color;
	class BaseCollider;
}

/// <summary>
/// 接続されたブロックグループを、接続した順に次のブロックへ最短距離で渡らせて1箇所に集め、
/// 上へ打ち上げるクラス。
/// ブロックの所有権は持たない(所有権は StageBlockField 側にある)。
/// 打ち上げたグループは足場ではなくなるため、集合開始時に StageBlockField の表からは外し、
/// 段の所有（ストリーミングによる破棄対象）からも切り離す。
/// </summary>
class BlockGroupLauncher{
public:

	/// <summary>調整値</summary>
	struct Params{
		float gatherSpeed;		// 集合地点へ向かって移動する速さ
		float launchSpeed;		// 打ち上げの初速
		float launchAccel;		// 打ち上げ中の加速度
		float launchLifeTime;	// 打ち上げてから制御を手放すまでの時間
		float blockSize;		// 押し戻し判定に使うブロック1個の大きさ
		float separationSpeed;	// 押し戻しでグループ同士が離れていく速さ
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

	// 打ち上げ用の座標系を unique_ptr で持つためコピーはできない。
	// BlockGroupLauncherManager が vector で使い回す時にムーブされるため、ムーブだけ残しておく
	BlockGroupLauncher(const BlockGroupLauncher&) = delete;
	BlockGroupLauncher& operator=(const BlockGroupLauncher&) = delete;
	BlockGroupLauncher(BlockGroupLauncher&&) noexcept = default;
	BlockGroupLauncher& operator=(BlockGroupLauncher&&) noexcept = default;

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

	/// <summary>
	/// 打ち上げたブロックがボスに当たったことを通知する。
	/// 当たり判定の最中にブロックを消すと Collider の列挙中に実体が消えてしまうため、
	/// ここでは破棄を予約するだけにして、実際の破棄は次の Update() で行う。
	/// </summary>
	/// <returns>
	/// 初めて通知された場合のみ true。
	/// 同じ塊の別のブロックが同じフレームに当たった場合(2回目以降)や、動いていない場合は false
	/// </returns>
	bool NotifyBossHit();

	/// <summary>
	/// 集合・打ち上げ中のグループ同士を、接続順に線で結んで描画する
	/// </summary>
	/// <param name="color">線の色</param>
	/// <param name="thickness">太さ</param>
	void DrawConnectLine(const AOENGINE::Color& color,float thickness) const;

	/// <summary>
	/// 噴射パーティクルを出す位置(集めたブロックの下端中央)を計算する。
	/// 生きているブロックが1つも無い場合は outJetPos を書き換えない
	/// </summary>
	/// <param name="outJetPos">求めた噴射位置</param>
	void CalclateJetPos(Math::Vector3& outJetPos) const;

private:

	/// <summary>集合中のグループ1つ分の作業データ</summary>
	struct GatheringGroup{
		std::vector<Block*> blocks;			// グループのブロック(非所有)
		std::vector<Math::Vector3> offsets;	// 接続した時点のプレイヤー位置から見た各ブロックの相対位置
		std::vector<Math::Vector3> path;	// 次のブロック -> ... -> 集合地点 と直線で結んだ経路
		float pathLength = 0.0f;			// 経路の全長
		float progress = 0.0f;				// 経路上を進んだ距離
		Math::Vector3 basePoint{};			// 経路上の現在位置(押し戻す前の位置)
		Math::Vector3 separation{};			// 他のグループから押し戻された分のずらし量
		Math::Vector3 rootOffset{};			// 打ち上げ用の座標系(launchRoot_)の原点から見たグループの位置
	};

	/// <summary>request の targetIndex 番目のグループから作業データを組み立てる</summary>
	bool MakeGatheringGroup(const GatherRequest& request,size_t targetIndex,GatheringGroup& outGroup) const;
	/// <summary>集合中の更新。経路上の目標位置へブロックを動かす</summary>
	void UpdateGathering(float deltaTime);
	/// <summary>グループ同士の重なりを調べ、重なった分だけ互いに押し戻す</summary>
	void ResolveGroupSeparation(float deltaTime);
	/// <summary>
	/// 2つのグループの重なりから、groupB を groupA から引き離すのに必要な押し出し量を求める
	/// </summary>
	/// <param name="groupA">押し戻される側その1</param>
	/// <param name="originA">groupA の基準位置(この位置に offsets を足したものが各ブロックの位置)</param>
	/// <param name="groupB">押し戻される側その2</param>
	/// <param name="originB">groupB の基準位置</param>
	/// <param name="fallbackDirection">ぴったり重なって向きが決まらない時に逃がす向き</param>
	/// <param name="outPush">求めた押し出し量(groupB を動かす向き)</param>
	/// <returns>重なっていなければ false</returns>
	bool ComputeGroupPush(const GatheringGroup& groupA,const Math::Vector3& originA,
						  const GatheringGroup& groupB,const Math::Vector3& originB,
						  const Math::Vector3& fallbackDirection,Math::Vector3& outPush) const;
	/// <summary>
	/// 集合が終わった塊を1つの座標系(launchRoot_)にまとめる。
	/// 座標系の原点は噴射位置に置き、各グループはそこからの相対位置として保持し直す
	/// </summary>
	void BuildLaunchRoot();
	/// <summary>打ち上げ中の更新。座標系ごと上へ動かし、ブロックをそれに追従させる</summary>
	void UpdateLaunched(float deltaTime);
	/// <summary>持っているブロックを全て破棄して制御を手放す。ボスに当たった時に呼ぶ</summary>
	void DestroyBlocks();
	/// <summary>基準位置に合わせてグループのブロックを動かす</summary>
	void MoveGroup(const GatheringGroup& group,const Math::Vector3& basePoint,float deltaTime) const;

	/// <summary>経路上を distance だけ進んだ位置を求める</summary>
	static Math::Vector3 SamplePath(const std::vector<Math::Vector3>& path,float distance);
	/// <summary>経路の全長を求める</summary>
	static float ComputePathLength(const std::vector<Math::Vector3>& path);
	/// <summary>ブロックのワールド座標を取得する</summary>
	static Math::Vector3 GetBlockPosition(const Block* block);
	/// <summary>ぴったり重なった時に逃がす向き。集合地点の周りへ均等に配る</summary>
	static Math::Vector3 MakeFallbackDirection(size_t index,size_t groupCount);
	/// <summary>グループに属するブロックの中心(平均位置)を求める</summary>
	static bool TryGetGroupCenter(const GatheringGroup& group,Math::Vector3& outCenter);
	/// <summary>ブロックを diff だけ動かす。Rigidbodyがあれば速度で、無ければ座標を直接動かす</summary>
	static void MoveBlock(Block* block,const Math::Vector3& diff,float deltaTime);
private:
	State state_ = State::Idle;

	// 集めたブロック全体をまとめる座標系。打ち上げ中はこれだけを動かし、
	// ブロックと演出オブジェクトの両方をこの座標系へ追従させる(演出側はこれを親にする)
	std::unique_ptr<AOENGINE::WorldTransform> launchRoot_;

	// 打ち上げ演出(噴射パーティクルなど)をまとめて持つ。
	// launchRoot_ より後ろに宣言することで、破棄順が「演出オブジェクト -> launchRoot_」になる。
	// 演出側は launchRoot_ を親として参照しているため、この順でないと消えかけの座標系を参照してしまう
	EffectObjectGroup launchEffects_;

	std::vector<GatheringGroup> groups_;	// 集合・打ち上げの対象

	float launchVelocityY_ = 0.0f;	// 現在の打ち上げ速度
	float launchTimer_ = 0.0f;		// 制御を手放すまでの残り時間

	bool isBossHit_ = false;		// ボスに当たったか(次の Update() でブロックを破棄する)

	Params params_{};

	StageBlockField* pField_ = nullptr;	// 非所有

public: // accessor

	void SetField(StageBlockField* field){ pField_ = field; }

	State GetState() const{ return state_; }
	bool IsActive() const{ return state_ != State::Idle; }
	// 集合・打ち上げの対象になっているグループ数
	int GetGroupCount() const{ return static_cast<int>(groups_.size()); }

	int GetBlockCount() const;

	/// <summary>指定したColliderが、このランチャーが動かしているブロックのものか</summary>
	bool HasCollider(const AOENGINE::BaseCollider* collider) const;

};
