#pragma once
#include <array>
#include "Engine/Lib/Math/MathStructures.h"

namespace Math {

struct Plane {
	Math::Vector3 normal = CVector3::ZERO;
	float distance = 0.0f;

	float SignedDistance(const Math::Vector3& point) const;
	void Normalize();
};

class Frustum final {
public:
	enum PlaneIndex {
		Left,
		Right,
		Bottom,
		Top,
		Near,
		Far,
		Count
	};

	static Math::Frustum FromViewProjection(const Math::Matrix4x4& viewProjection);

	bool Intersects(const Math::Sphere& sphere) const;

	const std::array<Math::Plane, Count>& GetPlanes() const { return planes_; }

private:
	std::array<Math::Plane, Count> planes_{};
};

}
