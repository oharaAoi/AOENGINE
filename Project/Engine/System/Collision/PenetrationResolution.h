#pragma once
#include "Engine/Lib/Math/MathStructures.h"
#include <variant>

Vector3 PenetrationResolution(const Sphere& s1, const Sphere& s2);

Vector3 PenetrationResolutionAABBandSphere(const AABB& aabb, const Sphere& s1);

Vector3 PenetrationResolutionAABBandAABB(const AABB& aabb1, const AABB& aabb2);

//================================================================================================//
//								当たり判定の呼び出し関数群											　//
//================================================================================================//

Vector3 PenetrationResolution(const Sphere& s, const AABB& aabb);
Vector3 PenetrationResolution(const AABB& aabb, const Sphere& s);

Vector3 PenetrationResolution(const AABB& aabb1, const AABB& aabb2);

Vector3 PenetrationResolution(const std::variant<Sphere, AABB, OBB>& shape1,
                              const std::variant<Sphere, AABB, OBB>& shape2);
