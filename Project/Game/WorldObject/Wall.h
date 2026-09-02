#pragma once

#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Game/Stage/GridPos.h"

/// <summary>
/// ステージ上の、連結・打ち上げの対象にならない足場 としての tagクラス
/// Block と違い StageBlockField には登録しないため、乗ってもグループには接続されない
/// </summary>
class Wall : public AOENGINE::BaseEntity{
public:
	Wall() = default;
	~Wall() = default;

private:

	/// グローバルグリッド座標（Blockと同じ座標系）
	GridPos gridPos_{};

public:
	/// <summary>グローバルグリッド座標を取得する</summary>
	const GridPos& GetGridPos() const{ return gridPos_; }
	/// <summary>グローバルグリッド座標を設定する</summary>
	void SetGridPos(const GridPos& pos){ gridPos_ = pos; }
};
