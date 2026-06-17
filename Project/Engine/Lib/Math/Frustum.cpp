#include "Frustum.h"

#include <cmath>

using namespace Math;

float Plane::SignedDistance(const Math::Vector3& point) const {
	return normal.x * point.x + normal.y * point.y + normal.z * point.z + distance;
}

void Plane::Normalize() {
	const float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
	if (length == 0.0f) {
		return;
	}

	normal = normal / length;
	distance /= length;
}

Math::Frustum Frustum::FromViewProjection(const Math::Matrix4x4& viewProjection) {
	Math::Frustum frustum{};

	// This matrix uses row-vector multiplication, so clip planes are extracted from columns.
	frustum.planes_[Left] = Math::Plane{
		.normal = Math::Vector3(
			viewProjection.m[0][0] + viewProjection.m[0][3],
			viewProjection.m[1][0] + viewProjection.m[1][3],
			viewProjection.m[2][0] + viewProjection.m[2][3]),
		.distance = viewProjection.m[3][0] + viewProjection.m[3][3],
	};
	frustum.planes_[Right] = Math::Plane{
		.normal = Math::Vector3(
			viewProjection.m[0][3] - viewProjection.m[0][0],
			viewProjection.m[1][3] - viewProjection.m[1][0],
			viewProjection.m[2][3] - viewProjection.m[2][0]),
		.distance = viewProjection.m[3][3] - viewProjection.m[3][0],
	};
	frustum.planes_[Bottom] = Math::Plane{
		.normal = Math::Vector3(
			viewProjection.m[0][1] + viewProjection.m[0][3],
			viewProjection.m[1][1] + viewProjection.m[1][3],
			viewProjection.m[2][1] + viewProjection.m[2][3]),
		.distance = viewProjection.m[3][1] + viewProjection.m[3][3],
	};
	frustum.planes_[Top] = Math::Plane{
		.normal = Math::Vector3(
			viewProjection.m[0][3] - viewProjection.m[0][1],
			viewProjection.m[1][3] - viewProjection.m[1][1],
			viewProjection.m[2][3] - viewProjection.m[2][1]),
		.distance = viewProjection.m[3][3] - viewProjection.m[3][1],
	};
	frustum.planes_[Near] = Math::Plane{
		.normal = Math::Vector3(
			viewProjection.m[0][2],
			viewProjection.m[1][2],
			viewProjection.m[2][2]),
		.distance = viewProjection.m[3][2],
	};
	frustum.planes_[Far] = Math::Plane{
		.normal = Math::Vector3(
			viewProjection.m[0][3] - viewProjection.m[0][2],
			viewProjection.m[1][3] - viewProjection.m[1][2],
			viewProjection.m[2][3] - viewProjection.m[2][2]),
		.distance = viewProjection.m[3][3] - viewProjection.m[3][2],
	};

	for (Math::Plane& plane : frustum.planes_) {
		plane.Normalize();
	}

	return frustum;
}

bool Frustum::Intersects(const Math::Sphere& sphere) const {
	for (const Math::Plane& plane : planes_) {
		if (plane.SignedDistance(sphere.center) < -sphere.radius) {
			return false;
		}
	}

	return true;
}
