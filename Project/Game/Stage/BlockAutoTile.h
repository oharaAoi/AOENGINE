#pragma once

/// stl
#include <cstdint>

/// <summary>
/// 4近傍の接続を表すビット。
/// 「埋まっている」= その方向に Block か StepBlock か Wall がある。
/// </summary>
enum BlockNeighborBit : uint8_t{
	kBlockNeighborUp = 1 << 0,
	kBlockNeighborDown = 1 << 1,
	kBlockNeighborLeft = 1 << 2,
	kBlockNeighborRight = 1 << 3,
};

/// <summary>
/// 隣接パターン(0〜15)に対して適用する見た目。
/// modelName は AssetsManager が登録した拡張子付きファイル名をそのまま渡す。
/// </summary>
struct BlockTileAppearance{
	/// <summary>ModelManager に登録されているモデル名(ファイル名)</summary>
	const char* modelName;
	/// <summary>Z軸まわりの回転(度)</summary>
	float rotateZDegree;
};

/// <summary>
/// ブロックのオートタイル(周囲の隣接状況によるモデル・回転の切り替え)に関する対応表を持つモジュール。
/// StageBlockField から独立させてあるので、モデルを追加・変更したいときはこのファイルだけ直せばよい。
/// </summary>
namespace BlockAutoTile{

	/// <summary>
	/// 隣接マスク(0〜15。BlockNeighborBit の組み合わせ)から適用する見た目を引く。
	/// </summary>
	/// <param name="neighborMask">4近傍の埋まり具合を表すビットマスク</param>
	/// <returns>モデル名と回転角</returns>
	const BlockTileAppearance& GetAppearance(uint8_t neighborMask);

}
