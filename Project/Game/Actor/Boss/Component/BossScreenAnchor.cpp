#include "BossScreenAnchor.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "Engine/Lib/Math/MyMatrix.h"
#include "Engine/WinApp/WinApp.h"

Math::Vector3 BossScreenAnchor::Solve(const Math::Matrix4x4& viewProjection, const Params& params) const {

	// 画面幅と高さ。0除算を避けるため最低1にクランプする
	const float width = static_cast<float>((std::max<std::uint32_t>)(1u, AOENGINE::WinApp::sClientWidth));
	const float height = static_cast<float>((std::max<std::uint32_t>)(1u, AOENGINE::WinApp::sClientHeight));

	// スクリーン座標からNDC
	const float ndcX = params.screenPos.x / width * 2.0f - 1.0f;
	const float ndcY = 1.0f - params.screenPos.y / height * 2.0f;

	// NDC → ワールドのレイ
	const Math::Matrix4x4 invViewProjection = viewProjection.Inverse();
	const Math::Vector3 nearPoint = TransformCoord({ ndcX, ndcY, 0.0f }, invViewProjection);
	const Math::Vector3 farPoint = TransformCoord({ ndcX, ndcY, 1.0f }, invViewProjection);

	//  レイと z = worldZ 平面の交点
	const Math::Vector3 rayDir = farPoint - nearPoint;
	if (std::abs(rayDir.z) < 1e-6f) {
		// カメラが真横を向いていて平面と交わらない
		return Math::Vector3(nearPoint.x, nearPoint.y, params.worldZ);
	}
	const float t = (params.worldZ - nearPoint.z) / rayDir.z;
	return nearPoint + rayDir * t;
}
