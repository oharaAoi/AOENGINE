#pragma once
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Matrix4x4.h"

/// <summary>
/// スクリーン座標上の固定Yにボスが貼り付いて見えるワールド座標を求める
/// </summary>
class BossScreenAnchor {
public:

	struct Params {
		Math::Vector2 screenPos;	// 画面上での固定位置
		float worldZ;				// ボスが乗るワールドZ平面
	};

	BossScreenAnchor() = default;
	~BossScreenAnchor() = default;

	/// <summary>
	/// 現在のカメラでscreenPosに見えるワールド座標を返す
	/// </summary>
	Math::Vector3 Solve(const Math::Matrix4x4& viewProjection, const Params& params) const;
};
