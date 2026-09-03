#pragma once

#include "Engine/Lib/Math/Matrix4x4.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"

/// <summary>
/// スクリーン座標を、指定したワールドZ平面上の座標へ変換する共通コンポーネント。
/// </summary>
class ScreenWorldPlaneAnchor {
public:
	struct Params {
		Math::Vector2 screenPosition;
		float worldZ = 0.0f;
	};

	Math::Vector3 Solve(const Math::Matrix4x4& viewProjection, const Params& params) const;
};
