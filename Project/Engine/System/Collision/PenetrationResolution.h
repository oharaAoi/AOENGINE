#pragma once
#include "Engine/Lib/Math/MathStructures.h"
#include <variant>

Math::Vector3 PenetrationResolution(const Math::Sphere& s1, const Math::Sphere& s2);

Math::Vector3 PenetrationResolutionAABBandSphere(const Math::AABB& aabb, const Math::Sphere& s1);

Math::Vector3 PenetrationResolutionAABBandAABB(const Math::AABB& aabb1, const Math::AABB& aabb2);

//================================================================================================//
//								当たり判定の呼び出し関数群											　//
//================================================================================================//

template<typename T1, typename T2>
Math::Vector3 PenetrationResolution(const T1&, const T2&) {
    return CVector3::ZERO;
}

Math::Vector3 PenetrationResolution(const Math::Sphere& s, const Math::AABB& aabb);
Math::Vector3 PenetrationResolution(const Math::AABB& aabb, const Math::Sphere& s);

Math::Vector3 PenetrationResolution(const Math::AABB& aabb1, const Math::AABB& aabb2);

Math::Vector3 PenetrationResolution(const std::variant<Math::Sphere, Math::AABB, Math::OBB, Math::Line>& shape1,
                              const std::variant<Math::Sphere, Math::AABB, Math::OBB, Math::Line>& shape2);