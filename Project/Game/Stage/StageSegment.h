#pragma once

/// stl
#include <array>
#include <string>

/// game
#include "Game/Stage/GridPos.h"

/// <summary>
///  CSVから読み込んだ、1セグメント分のブロック配置データ。
///  ファイルからデータへの橋渡しに専念し、World上の実体は持たない
///  （実体の生成・所有は StageBlockField が行う）。
/// </summary>
class StageSegment{
public:
	StageSegment() = default;
	~StageSegment() = default;

	/// <summary>
	/// csvからブロックの配置データを読み込む
	/// </summary>
	/// <param name="filePath"></param>
	void LoadBlockData(const std::string& filePath);

	/// <summary>
	/// 配置データの1マスを取得する。範囲外なら 0（空マス）を返す。
	/// </summary>
	/// <param name="row">行インデックス（0が最上段。CSVの並びそのまま）</param>
	/// <param name="col">列インデックス</param>
	int GetCell(int row,int col) const;

private:
	/// ブロックの配置データ [行][列]
	std::array<std::array<int,kBlockCol>,kBlockRow> blockData_{};
};
