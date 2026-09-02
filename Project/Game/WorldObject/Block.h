#pragma once

#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Game/Stage/GridPos.h"

/// <summary>
/// ステージ上の足場 としての tagクラス
/// </summary>
class Block : public AOENGINE::BaseEntity{
public:
	Block() = default;
	~Block() = default;

private:

	bool isConnected = false;

	/// グローバルグリッド座標（StageBlockField 上での位置）
	GridPos gridPos_{};
	/// 所属している連結グループのID（StageBlockField が発行・管理する）
	int groupId_ = -1;
	/// 所属グループのメンバー配列内での自分のインデックス（StageBlockField が管理する値）
	int groupIndex_ = -1;

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
};