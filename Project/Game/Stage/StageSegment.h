#pragma once

/// stl
#include <array>
#include <string>

static const int kBlockCol = 11;
static const int kBlockRow = 11;

/// <summary>
///  CSVから読み込んだ、複数の Block が配置されている大枠のデータ
/// </summary>
class StageSegment{
public:
	/// <summary>
	/// csvからブロックの配置データを読み込む
	/// </summary>
	/// <param name="filePath"></param>
	void LoadBlockData(const std::string& filePath);
	/// <summary>
	/// 読み込んだブロックの配置データを元に、World上にブロックを配置する
	/// </summary>
	void SetupSegmentOnWorld();

private:
	/// ブロックの配置データ [行][列]
	std::array<std::array<int,kBlockCol>,kBlockRow> blockData_;
};

