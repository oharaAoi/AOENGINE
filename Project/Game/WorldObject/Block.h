#pragma once

#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Game/Stage/GridPos.h"

namespace {
	constexpr float kBlockSize = 1.0f;
}

/// <summary>
/// ステージ上の足場 としての tagクラス
/// </summary>
class Block : public AOENGINE::BaseEntity{
public:
	Block() = default;
	~Block() = default;

	void OnBrokenEffect() const;

private:

	bool isConnected = false;

	/// グローバルグリッド座標（StageBlockField 上での位置）
	GridPos gridPos_{};
	/// 所属している連結グループのID（StageBlockField が発行・管理する）
	int groupId_ = -1;
	/// 所属グループのメンバー配列内での自分のインデックス（StageBlockField が管理する値）
	int groupIndex_ = -1;

	/// オートタイルで最後に適用した隣接マスク（StageBlockField が管理。未適用なら-1）
	int tileMask_ = -1;

public:
	bool GetIsConnected() const{ return isConnected; }
	void SetIsConnected(bool value){ isConnected = value; }

	/// <summary>グローバルグリッド座標を取得する</summary>
	const GridPos& GetGridPos() const{ return gridPos_; }
	/// <summary>グローバルグリッド座標を設定する</summary>
	void SetGridPos(const GridPos& pos){ gridPos_ = pos; }

	/// <summary>所属している連結グループのIDを取得する</summary>
	int GetGroupId() const{ return groupId_; }
	/// <summary>所属している連結グループのIDを設定する</summary>
	void SetGroupId(int groupId){ groupId_ = groupId; }

	/// <summary>所属グループ内での自分のインデックスを取得する（StageBlockField が管理）</summary>
	int GetGroupIndex() const{ return groupIndex_; }
	/// <summary>所属グループ内での自分のインデックスを設定する（StageBlockField が管理）</summary>
	void SetGroupIndex(int groupIndex){ groupIndex_ = groupIndex; }

	/// <summary>オートタイルで最後に適用した隣接マスクを取得する（未適用なら-1）</summary>
	int GetTileMask() const{ return tileMask_; }
	/// <summary>オートタイルで最後に適用した隣接マスクを設定する</summary>
	void SetTileMask(int tileMask){ tileMask_ = tileMask; }
};