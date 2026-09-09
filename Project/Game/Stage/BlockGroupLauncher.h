#pragma once

/// stl
#include <memory>
#include <string>
#include <vector>

/// engine
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Module/Components/WorldTransform.h"

/// game
#include "Game/Effect/EffectObjectGroup.h"
#include "Game/Path/LinePathMover.h"

class Block;
class StageBlockField;
class ComboTextUIManager;
class DamageTextUIManager;
class BlockDamageCalculator;

namespace AOENGINE{
	class Color;
	class BaseCollider;
}

/// <summary>
/// 接続されたブロックグループを、接続した順に次のブロックへ最短距離で渡らせて1箇所に集め、
/// 打ち上げるクラス。
/// 打ち上げ後は LinePathMover で経路上を進ませる。Line が指定されていればその形を辿り、
/// 指定が無ければ真上へ伸びる直線を辿る(今まで通りまっすぐ上へ上がる)。
/// 狙い先(ボス)が渡されている場合は、始点を固定したまま経路全体を狙い先へ向け直し続けるため、
/// 経路の形を保ったまま必ず狙い先へ届く。
/// 動き出すのは最初に接続したグループだけで、後ろのグループは1つ前のグループが自分に触れるまで
/// 接続地点で待つ。玉突きのように順番に動き出し、そのまま数珠つなぎで集合地点へ向かう。
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
		float launchMaxSpeed = 0.0f;	// 打ち上げ中の速さの上限(0以下なら上限なし)
		float launchHomingRate = 0.0f;	// 狙い先へ経路を寄せる速さ(0以下なら即座に寄せる)
		float launchRotateRate = 0.0f;	// 進む向きへ塊を傾ける速さ(0以下なら即座に向く)
		// 打ち上げで辿る Line(LineLoader が読み込んだもの)の名前。
		// 空、または読み込まれていない場合は真上へ伸びる直線を辿る
		std::string launchLineName;
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
	/// 集めたブロックを打ち上げる。集合しきった位置を始点にした経路を組み立て、その上を進ませる
	/// </summary>
	void Launch();

	/// <summary>
	/// 打ち上げの狙い先(ボス)を伝える。打ち上げた時に狙い先を持っていた場合、
	/// 経路の終点がここへ追従し続けるため必ず狙い先へ届く
	/// </summary>
	/// <param name="position">狙い先のワールド座標</param>
	void SetTarget(const Math::Vector3& position);

	/// <summary>
	/// 狙い先を外す。飛んでいる最中に外した場合は、その時に向いていた先へそのまま飛んでいく
	/// </summary>
	void ClearTarget();

	/// <summary>
	/// 更新。集合中は集合の経路上を、打ち上げ中は打ち上げの経路上へブロックを動かす
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
		int groupId = -1;					// 元のグループID。コンボ表示を引き当てるのに使う
		std::vector<Block*> blocks;			// グループのブロック(非所有)
		std::vector<Math::Vector3> offsets;	// 接続した時点のプレイヤー位置から見た各ブロックの相対位置
		std::vector<Math::Vector3> path;	// 次のブロック -> ... -> 集合地点 と直線で結んだ経路
		float pathLength = 0.0f;			// 経路の全長
		float progress = 0.0f;				// 経路上を進んだ距離
		Math::Vector3 basePoint{};			// 経路上の現在位置(押し戻す前の位置)
		Math::Vector3 separation{};			// 他のグループから押し戻された分のずらし量
		Math::Vector3 rootOffset{};			// 打ち上げ用の座標系(launchRoot_)の原点から見たグループの位置
		// 打ち上げた時点の各ブロックの向き。塊ごと進む向きへ回すため、この向きに回転を掛けて使う
		std::vector<Math::Quaternion> launchRotates;
		bool isMoving = false;				// 経路上を動き出しているか(待機中は接続地点から動かない)
		float releaseProgress = 0.0f;		// 1つ前のグループがこの距離まで進んだら動き出す(塊同士が触れる位置)
	};

	/// <summary>request の targetIndex 番目のグループから作業データを組み立てる</summary>
	bool MakeGatheringGroup(const GatherRequest& request,size_t targetIndex,GatheringGroup& outGroup) const;
	/// <summary>
	/// 各グループが「1つ前のグループがどこまで進んだら動き出すか」を決める。
	/// 経路も塊の形も集合中は変わらないため、集合開始時に1度だけ求める
	/// </summary>
	void SetupReleaseProgress();
	/// <summary>集合中の更新。経路上の目標位置へブロックを動かす</summary>
	void UpdateGathering(float deltaTime);
	/// <summary>接続地点で待機しているグループのうち、1つ前のグループが触れる所まで来たものを動かし始める</summary>
	void UpdateGatherRelease(float deltaTime);
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
	/// <summary>
	/// 打ち上げで辿る経路を LinePathMover へ渡す。
	/// 打ち上げの初速・加速度はそのまま経路上を進む速さとして使う
	/// </summary>
	/// <param name="startPos">経路の始点(集合しきった塊の噴射位置)</param>
	void BuildLaunchPath(const Math::Vector3& startPos);
	/// <summary>
	/// 打ち上げで辿る座標を作る。Line が指定されていればその形を、
	/// 無ければ真上へ伸びる直線を、startPos が始点に来るように平行移動して返す
	/// </summary>
	/// <param name="startPos">経路の始点</param>
	std::vector<Math::Vector3> MakeLaunchPoints(const Math::Vector3& startPos) const;
	/// <summary>打ち上げ中の更新。座標系を経路上の位置へ動かし、ブロックをそれに追従させる</summary>
	void UpdateLaunched(float deltaTime);
	/// <summary>
	/// 向けている先を狙い先へ寄せる。カメラの揺れがそのまま経路へ乗らないように少しずつ寄せ、
	/// 終点に近づくほど強く寄せて、着く時には必ず狙い先と一致させる
	/// </summary>
	void UpdateLaunchAim(float deltaTime);
	/// <summary>
	/// 経路上の座標を、狙い先へ向け直した後の座標へ変換する。
	/// 始点を固定したまま、経路の終点が今向けている先に来るように回して伸ばす(相似変換)
	/// </summary>
	/// <param name="pathPosition">経路上の座標</param>
	Math::Vector3 ApplyLaunchAim(const Math::Vector3& pathPosition) const;
	/// <summary>
	/// 今の進む向きを求める。経路の少し先を見て、狙い先へ向け直した後の座標の差から求める
	/// </summary>
	/// <param name="rootPos">今の座標系の位置(向け直した後)</param>
	/// <returns>進む向き(単位ベクトル)。求められない場合は今の向きをそのまま返す</returns>
	Math::Vector3 CalclateLaunchDirection(const Math::Vector3& rootPos) const;
	/// <summary>
	/// 塊を進む向きへ傾ける回転を進める。打ち上げた瞬間に倒れると不自然なため、少しずつ向ける
	/// </summary>
	void UpdateLaunchRotate(float deltaTime);
	/// <summary>
	/// 経路の終点を持っていきたい位置。ブロックの塊の中心が狙い先へ来るように、
	/// 座標系の原点から見た塊の中心のぶんだけずらして返す
	/// </summary>
	Math::Vector3 GetAimGoal() const;
	/// <summary>持っているブロックを全て破棄して制御を手放す。ボスに当たった時に呼ぶ</summary>
	void DestroyBlocks();
	/// <summary>
	/// 集合を始めた時に切った重力を戻す。
	/// 狙い先へ着いたのに当たり判定が入らなかった時、空中で止まって見えないようにするための保険
	/// </summary>
	void ReleaseBlocksWithGravity();
	/// <summary>
	/// 基準位置に合わせてグループのブロックを動かす。
	/// rotate を渡すと、基準位置からの相対位置とブロックの向きの両方をその分だけ回す
	/// </summary>
	/// <param name="group">動かすグループ</param>
	/// <param name="basePoint">基準位置</param>
	/// <param name="rotate">塊ごと回す回転(集合中は回さないので単位回転)</param>
	/// <param name="deltaTime">経過時間</param>
	void MoveGroup(const GatheringGroup& group,const Math::Vector3& basePoint,
				   const Math::Quaternion& rotate,float deltaTime) const;
	/// <summary>
	/// グループが経路上を動き出した時の処理。
	/// そのグループのコンボ表示を消し始め、集めた数へこのグループの分を足し込む。
	/// 全グループが動き出したら(=集合しきったら)総ダメージを出す
	/// </summary>
	void NotifyGatherStarted(const GatheringGroup& group);
	/// <summary>指定したグループのコンボ表示を消し始める</summary>
	void FadeOutComboText(int groupId) const;
	/// <summary>
	/// そこまでに集めたブロック数とコンボを集合地点へ出す。
	/// 既に出ている場合は数が書き換わり、その場でもう一度跳ねる
	/// </summary>
	void ShowGatheredCount() const;
	/// <summary>
	/// 集めた数から総ダメージを求めて集合地点へ出す。1回の集合につき1度だけ出す
	/// </summary>
	void ShowGatherDamage();
	/// <summary>
	/// まだ動き出していないグループを全て動き出させる。
	/// 打ち上げると集合の更新が止まるため、放っておくとコンボ表示が消えないまま残る
	/// </summary>
	void ReleaseRemainingGroups();

	/// <summary>経路上を distance だけ進んだ位置を求める</summary>
	static Math::Vector3 SamplePath(const std::vector<Math::Vector3>& path,float distance);
	/// <summary>経路の全長を求める</summary>
	static float ComputePathLength(const std::vector<Math::Vector3>& path);
	/// <summary>
	/// 待機しているグループの塊に、近づいてくるグループの塊がちょうど触れる時の残り距離を求める。
	/// ブロックはどれも同じ大きさの箱なので、箱同士が重なる残り距離の範囲を軸ごとに解いて求める
	/// </summary>
	/// <param name="approaching">近づいてくる側(1つ前のグループ)</param>
	/// <param name="waiting">接続地点で待っている側</param>
	/// <param name="approachDirection">approaching が waiting へ入ってくる向き(単位ベクトル)</param>
	/// <param name="blockSize">ブロック1個の大きさ</param>
	/// <returns>触れる瞬間の、waiting の接続地点までの残り距離。触れ合わない場合は 0</returns>
	static float ComputeContactDistance(const GatheringGroup& approaching,const GatheringGroup& waiting,
										const Math::Vector3& approachDirection,float blockSize);
	/// <summary>ブロックのワールド座標を取得する</summary>
	static Math::Vector3 GetBlockPosition(const Block* block);
	/// <summary>
	/// baseDirection から aimDirection へ向ける回転。
	/// ほぼ同じ向き / 真逆の時に計算が壊れないようにする
	/// </summary>
	static Math::Quaternion MakeAimRotation(const Math::Vector3& baseDirection,const Math::Vector3& aimDirection);
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

	// 打ち上げ中に辿る経路。集合しきった位置を始点にして、その上を加速しながら進む
	LinePathMover launchMover_;

	float launchTimer_ = 0.0f;		// 制御を手放すまでの残り時間
	// 終点へ着いてから制御を手放すまでの残り時間。
	// 着いた瞬間に手放すと、当たり判定が入る前にランチャーから切り離されてダメージが乗らない
	float arrivalTimer_ = 0.0f;

	// 狙い先(ボス)のワールド座標。飛んでいる最中も外から毎フレーム更新される
	Math::Vector3 targetPosition_{};
	bool hasTarget_ = false;

	// 打ち上げた時に組み立てた経路の始点と終点。狙い先へ向け直す変換の基準に使う
	Math::Vector3 launchPathStart_{};
	Math::Vector3 launchPathEnd_{};
	// 今向けている先。狙い先へ少しずつ寄せていく
	Math::Vector3 aimPosition_{};
	// 座標系の原点から見た塊の中心。中心が狙い先へ来るように向けるのに使う
	Math::Vector3 launchCenterOffset_{};
	// 打ち上げた時に狙い先を持っていたか。持っていた場合は寿命で打ち切らず、必ず狙い先まで飛ばす
	bool isHoming_ = false;

	// 今の進む向き。塊と演出をこの向きへ向ける(打ち上げ前は真上)
	Math::Vector3 launchDirection_ = CVector3::UP;
	// 塊全体(ブロック・座標系・演出)に掛ける回転。真上を向いた状態から進む向きへ傾けていく
	Math::Quaternion launchRotate_{};

	bool isBossHit_ = false;		// ボスに当たったか(次の Update() でブロックを破棄する)

	int currentGatherIndex_ = 0;	// 集合中のグループのうち、経路上を動かしているのはどれか(0～groups_.size()-1)

	Params params_{};

	StageBlockField* pField_ = nullptr;	// 非所有

	ComboTextUIManager* pComboTextUI_ = nullptr;	// 非所有。集合を始めたグループの表示を消すのに使う

	DamageTextUIManager* pDamageTextUI_ = nullptr;			// 非所有。集合しきった時に総ダメージを出すのに使う
	const BlockDamageCalculator* pDamageCalculator_ = nullptr;	// 非所有。実際に当たった時と同じ式で総ダメージを求める

	Math::Vector3 gatherPoint_{};	// 集合地点。総ダメージはここへ出す

	// 動き出したグループを数えた結果。動き出すたびに足し込み、全グループ分そろったら総ダメージを出す
	int gatheredBlockCount_ = 0;
	int gatheredGroupCount_ = 0;
	bool isDamageShown_ = false;	// この集合で総ダメージを出したか

public: // accessor

	void SetField(StageBlockField* field){ pField_ = field; }
	void SetComboTextUIManager(ComboTextUIManager* manager){ pComboTextUI_ = manager; }
	void SetDamageTextUIManager(DamageTextUIManager* manager){ pDamageTextUI_ = manager; }
	void SetDamageCalculator(const BlockDamageCalculator* calculator){ pDamageCalculator_ = calculator; }

	State GetState() const{ return state_; }
	bool IsActive() const{ return state_ != State::Idle; }
	// 集合・打ち上げの対象になっているグループ数
	int GetGroupCount() const{ return static_cast<int>(groups_.size()); }

	int GetBlockCount() const;

	/// <summary>指定したColliderが、このランチャーが動かしているブロックのものか</summary>
	bool HasCollider(const AOENGINE::BaseCollider* collider) const;

};
