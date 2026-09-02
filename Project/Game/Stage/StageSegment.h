#pragma once

/// stl
#include <array>
#include <string>

static const int kBlockCol = 10;
static const int kBlockRow = 10;

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
	std::array<std::array<int,kBlockRow>,kBlockCol> blockData_;
};

