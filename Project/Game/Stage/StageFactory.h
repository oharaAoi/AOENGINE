#pragma once

// game
#include "Game/Stage/StageBlockField.h"
#include "Game/Stage/StageSegment.h"

/// <summary>
/// ステージ自動を生産するためのクラス
/// </summary>
class StageFactory {
public:
	/// <summary>
	/// 生成関数
	/// </summary>
	/// <param name="field">セグメントを跨いでブロックを管理する連結グループ表</param>
	/// <param name="index">SegmentのIndex</param>
	/// <param name="addHeight">追加される座標の高さ</param>
	void Create(StageBlockField* field, StageSegment*segment, int index);
};

