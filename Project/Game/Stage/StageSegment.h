#pragma once

/// stl
#include <array>
#include <memory>
#include <string>
#include <vector>

class Block;
class StageBlockField;

static const int kBlockCol = 11;
static const int kBlockRow = 11;

/// <summary>
///  CSVから読み込んだ、複数の Block が配置されている大枠のデータ
/// </summary>
class StageSegment{
public:
	// blocks_ が std::unique_ptr<Block>（Block は前方宣言のみ）を保持するため、
	// 特殊メンバ関数をヘッダでインライン生成させず、Block の完全型が見える
	// StageSegment.cpp 側で定義する（unique_ptr + 前方宣言の定石）。
	// コンストラクタも例外時のメンバー巻き戻しでデストラクタを要求するため、
	// デストラクタだけでなくコンストラクタも .cpp 側に出す必要がある。
	StageSegment();
	~StageSegment();

	// unique_ptr を持つためコピー不可。ムーブは将来コンテナ（例: std::vector<StageSegment>）
	// で扱えるように明示的に宣言し、定義は .cpp 側に置く。
	StageSegment(const StageSegment&) = delete;
	StageSegment& operator=(const StageSegment&) = delete;
	StageSegment(StageSegment&&) noexcept;
	StageSegment& operator=(StageSegment&&) noexcept;

	/// <summary>
	/// csvからブロックの配置データを読み込む
	/// </summary>
	/// <param name="filePath"></param>
	void LoadBlockData(const std::string& filePath);
	/// <summary>
	/// 読み込んだブロックの配置データを元に、World上にブロックを配置する。
	/// 生成した Block は field に登録され、グリッド座標を通じて上下左右のブロックと連結される。
	/// </summary>
	/// <param name="field">セグメントを跨いでブロックを管理する連結グループ表</param>
	/// <param name="segmentOriginGx">このセグメントの原点となるグローバルグリッドX座標</param>
	void SetupSegmentOnWorld(StageBlockField* field,int segmentOriginGx, int addHeight = 0);

	/// <summary>
	/// このセグメントが生成した全ブロックを field の表から外し、GameObject を破棄する。
	/// 注意: セグメント破棄は連結グループの一部だけを消すことになるが、
	/// 画面外（ストリーミングで既に見えなくなった範囲）でのみ行われるため、
	/// 残りの連結性についての分割(split)検査はここでは行わない。
	/// </summary>
	/// <param name="field">セグメントを跨いでブロックを管理する連結グループ表</param>
	void UnregisterFromWorld(StageBlockField* field);

private:
	/// ブロックの配置データ [行][列]
	std::array<std::array<int,kBlockCol>,kBlockRow> blockData_;

	/// このセグメントが生成した Block（所有権はこのセグメントが持つ）
	std::vector<std::unique_ptr<Block>> blocks_;
};

