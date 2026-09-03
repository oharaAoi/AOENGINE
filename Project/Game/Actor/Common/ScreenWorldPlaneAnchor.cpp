#include "ScreenWorldPlaneAnchor.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "Engine/Lib/Math/MyMatrix.h"
#include "Engine/WinApp/WinApp.h"

Math::Vector3 ScreenWorldPlaneAnchor::Solve(
	const Math::Matrix4x4& viewProjection, const Params& params) const {
	const float width = static_cast<float>((std::max<std::uint32_t>)(1u, AOENGINE::WinApp::sClientWidth));
	const float height = static_cast<float>((std::max<std::uint32_t>)(1u, AOENGINE::WinApp::sClientHeight));

	const float ndcX = params.screenPosition.x / width * 2.0f - 1.0f;
	const float ndcY = 1.0f - params.screenPosition.y / height * 2.0f;
	const Math::Matrix4x4 inverseViewProjection = viewProjection.Inverse();
	const Math::Vector3 nearPoint = TransformCoord({ ndcX, ndcY, 0.0f }, inverseViewProjection);
	const Math::Vector3 farPoint = TransformCoord({ ndcX, ndcY, 1.0f }, inverseViewProjection);
	const Math::Vector3 rayDirection = farPoint - nearPoint;

	if (std::abs(rayDirection.z) < 1e-6f) {
		return { nearPoint.x, nearPoint.y, params.worldZ };
	}
	const float distance = (params.worldZ - nearPoint.z) / rayDirection.z;
	return nearPoint + rayDirection * distance;
}
