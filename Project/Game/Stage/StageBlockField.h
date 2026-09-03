#pragma once

/// stl
#include <unordered_map>
#include <vector>

/// game
#include "Game/Stage/GridPos.h"

/// engine
#include "Engine/Lib/Math/Vector3.h"

class Block;

namespace AOENGINE {
class Color;
}

/// <summary>
/// 複数のステージセグメントを横断して、ブロックのグリッド座標と
/// 上下左右で連結しているブロック群（グループ）を管理する常駐クラス。
/// このクラスはブロックを所有しない（非所有ポインタの表）。
/// </summary>
class StageBlockField{
private:
	/// <summary>4近傍（上下左右）のオフセット</summary>
	static const GridPos kNeighborOffsets[4];
public:

	/// <summary>グループが存在しないことを表す無効なグループID</summary>
	static constexpr int kInvalidGroupId = -1;

	/// <summary>
	/// ブロックを1個、指定したグリッド座標に登録する。
	/// 4近傍（上下左右）に存在する既存ブロックと自動的に同じグループへ統合される。
	/// セグメント単位の flood fill はこの関数をループで呼ぶだけで実現する。
	/// </summary>
	/// <param name="block">登録するブロック（非所有）</param>
	/// <param name="pos">グローバルグリッド座標</param>
	void AddBlock(Block* block,const GridPos& pos);

	/// <summary>
	/// 指定したグループに属する全ブロックを表（cells_ / groups_）から削除する。
	/// 破壊はグループ丸ごとの単位でのみ起きる前提のため、分割(split)検査は行わない。
	/// ブロック自体の GameObject の破棄（Destroy）は行わない。呼び出し側の責務とする。
	/// </summary>
	/// <param name="groupId">削除するグループID</param>
	void RemoveGroup(int groupId);

	/// <summary>
	/// ブロックを1個だけ表（cells_ / groups_）から取り除く。
	/// 注意: このブロックが属していたグループの連結性が分割される可能性があるが、
	/// 今回は分割(split)検査・再ラベリングは実装しない
	/// （画面外に流れたセグメントを表から外す用途などを想定しており、実害が無い前提）。
	/// </summary>
	/// <param name="block">表から外すブロック</param>
	void RemoveBlockFromField(Block* block);

	/// <summary>
	/// 指定したグループIDのメンバー配列を取得する。
	/// </summary>
	/// <param name="groupId">グループID</param>
	/// <returns>見つからなければ nullptr</returns>
	const std::vector<Block*>* GetGroup(int groupId) const;

	/// <summary>
	/// 指定したグループに属する全ブロックの中心(平均位置)を求める。
	/// グループ同士を線で結ぶときの端点として使う。
	/// </summary>
	/// <param name="groupId">グループID</param>
	/// <param name="outCenter">求めた中心の格納先</param>
	/// <returns>グループが見つからない、または有効なブロックが1つも無ければ false</returns>
	bool TryGetGroupCenter(int groupId,Math::Vector3& outCenter) const;

	/// <summary>
	/// 指定したグリッド座標にあるブロックを取得する。
	/// </summary>
	/// <param name="pos">グローバルグリッド座標</param>
	/// <returns>無ければ nullptr</returns>
	Block* GetBlockAt(const GridPos& pos) const;

	/// <summary>
	/// 登録済みの全セル・全グループを消去する。
	/// </summary>
	void Clear();

	/// <summary>
	/// グリッド座標をワールド座標へ変換する。
	/// Xはセグメントの中央が原点に来るように寄せ、Yは一番下の段(y = 0)の下面が
	/// 高さ0になるように積む（一番下の段のブロックの中心は 0.5）。
	/// </summary>
	/// <param name="pos">グローバルグリッド座標</param>
	/// <returns>ワールド座標</returns>
	static Math::Vector3 GridToWorld(const GridPos& pos);

	/// <summary>
	/// ワールド座標をグリッド座標へ変換する(GridToWorldの逆変換)。
	/// </summary>
	/// <param name="worldPos">ワールド座標</param>
	/// <returns>グローバルグリッド座標</returns>
	static GridPos WorldToGrid(const Math::Vector3& worldPos);

	/// <summary>
	/// 指定したワールド空間のAABB範囲(min~max)と重なるグリッドマスに置かれた
	/// ブロックを列挙する。範囲は高々数マスを想定した簡易実装(全ブロック走査はしない)。
	/// </summary>
	/// <param name="worldMin">範囲の最小座標</param>
	/// <param name="worldMax">範囲の最大座標</param>
	/// <returns>重なっているブロック(非所有ポインタ)の配列</returns>
	std::vector<Block*> GetBlocksInWorldAABB(const Math::Vector3& worldMin, const Math::Vector3& worldMax) const;

	/// <summary>
	/// 指定したグループに属する全ブロックの色を変える。
	/// 接続されたグループを見た目で分かるようにする用途に使う。
	/// </summary>
	/// <param name="groupId">色を変えるグループID</param>
	/// <param name="color">設定する色</param>
	void SetGroupColor(int groupId,const AOENGINE::Color& color);

	/// <summary>
	/// デバッグ用: 連結グループごとに異なる色をブロックへ設定する。
	/// グルーピングが意図通りに効いているかを目視で確認するための機能。
	/// </summary>
	void ApplyDebugGroupColors();

private:

	/// <summary>
	/// fromId のグループのメンバーを toId のグループへ移し替えて統合する（weighted union）。
	/// fromId のグループは空になった時点で groups_ から削除される。
	/// </summary>
	/// <param name="fromId">統合される側（消える側）のグループID</param>
	/// <param name="toId">統合する側（残る側）のグループID</param>
	void MergeGroup(int fromId,int toId);

	/// <summary>
	/// ブロックを、現在所属しているグループの配列から swap-and-pop で取り除く。
	/// groupId_ / groupIndex_ を初期化し、空になったグループは groups_ から削除する。
	/// cells_ からは削除しない（呼び出し元が別途行う）。
	/// </summary>
	/// <param name="block">グループから取り除くブロック</param>
	void RemoveBlockFromGroup(Block* block);

	/// <summary>
	/// 指定ブロックが現在 cells_ に登録されている場合のみ、そのセルを削除する。
	/// （別のブロックが既に同じ座標を占有している場合に、それを消してしまうのを防ぐ）
	/// </summary>
	/// <param name="block">セルの持ち主として確認するブロック</param>
	void EraseCellIfOwnedBy(Block* block);

	/// <summary>デバッグ用: グループIDから見分けやすい色を生成する</summary>
	static AOENGINE::Color MakeDebugGroupColor(int groupId);

	/// <summary>グリッド座標 -> そこにあるブロック</summary>
	std::unordered_map<GridPos,Block*> cells_;
	/// <summary>グループID -> 所属ブロック配列</summary>
	std::unordered_map<int,std::vector<Block*>> groups_;
	/// <summary>次に発行するグループID（単調増加）</summary>
	int nextGroupId_ = 0;
};
