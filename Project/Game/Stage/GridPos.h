#pragma once

/// stl
#include <cstddef>
#include <functional>

/// <summary>
/// ステージ上のブロックが持つグローバルなグリッド座標。
/// ワールド座標はこの整数座標から導出する（浮動小数での隣接判定は行わない）。
/// </summary>
struct GridPos{
	int x = 0;
	int y = 0;
};

/// <summary>
/// GridPos の等価比較
/// </summary>
inline bool operator==(const GridPos& lhs,const GridPos& rhs){
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const GridPos& lhs,const GridPos& rhs){
	return !(lhs == rhs);
}

namespace std{

	/// <summary>
	/// GridPos を std::unordered_map 等のキーとして使うためのハッシュ特殊化。
	/// 2つの int を混ぜる素直なハッシュ。
	/// </summary>
	template<>
	struct hash<GridPos>{
		size_t operator()(const GridPos& pos) const noexcept{
			size_t hx = std::hash<int>()(pos.x);
			size_t hy = std::hash<int>()(pos.y);
			// boost::hash_combine 相当の混ぜ方
			return hx ^ (hy + 0x9e3779b9u + (hx << 6) + (hx >> 2));
		}
	};

}
