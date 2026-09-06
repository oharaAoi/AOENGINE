#include "BlockAutoTile.h"

namespace{

	/// <summary>
	/// 隣接マスク(0〜15) -> 見た目 の対応表。
	/// 添字がそのまま BlockNeighborBit の組み合わせ(隣接マスク)になっている。
	/// 使用するモデルは block_inside / block_leftSide / block_leftTop / block_topMid の4種類のみ
	/// (block3/5/7/8/9 は今回のスコープ外)。
	/// モデルは2×2×2サイズなので、実際の配置時に scale 0.5 を掛けて1マスに合わせる(StageBlockField側の責務)。
	/// </summary>
	static const BlockTileAppearance kTable[16] = {
		// mask 0: 隣接なし / 露出 上下左右 / 孤立(近似)
		{"block_leftTop.obj",0.0f},
		// mask 1: 隣接 上 / 露出 下左右 / 縦線の下端(近似)
		{"block_leftTop.obj",90.0f},
		// mask 2: 隣接 下 / 露出 上左右 / 縦線の上端(近似)
		{"block_leftTop.obj",0.0f},
		// mask 3: 隣接 上下 / 露出 左右 / 縦線の中間(近似)
		{"block_leftSide.obj",0.0f},
		// mask 4: 隣接 左 / 露出 上下右 / 横線の右端(近似)
		{"block_leftTop.obj",270.0f},
		// mask 5: 隣接 上左 / 露出 下右 / 右下の角(正確)
		{"block_leftTop.obj",180.0f},
		// mask 6: 隣接 下左 / 露出 上右 / 右上の角(正確)
		{"block_leftTop.obj",270.0f},
		// mask 7: 隣接 上下左 / 露出 右 / 右辺(正確)
		{"block_leftSide.obj",180.0f},
		// mask 8: 隣接 右 / 露出 上下左 / 横線の左端(近似)
		{"block_leftTop.obj",0.0f},
		// mask 9: 隣接 上右 / 露出 下左 / 左下の角(正確)
		{"block_leftTop.obj",90.0f},
		// mask 10: 隣接 下右 / 露出 上左 / 左上の角(正確)
		{"block_leftTop.obj",0.0f},
		// mask 11: 隣接 上下右 / 露出 左 / 左辺(正確)
		{"block_leftSide.obj",0.0f},
		// mask 12: 隣接 左右 / 露出 上下 / 横線の中間(近似)
		{"block_topMid.obj",0.0f},
		// mask 13: 隣接 上左右 / 露出 下 / 下辺(正確)
		{"block_topMid.obj",180.0f},
		// mask 14: 隣接 下左右 / 露出 上 / 上辺(正確)
		{"block_topMid.obj",0.0f},
		// mask 15: 隣接 上下左右 / 露出 なし / 内側(正確)
		{"block_inside.obj",0.0f},
	};

}

const BlockTileAppearance& BlockAutoTile::GetAppearance(uint8_t neighborMask){
	// 範囲外マスクが来た場合は安全側として内側(mask 15)にフォールバックする
	if(neighborMask >= 16){
		return kTable[15];
	}
	return kTable[neighborMask];
}
