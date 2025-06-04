#include "PenetrationResolution.h"
#include "Engine/Lib/Math/MyMath.h"

Vector3 PenetrationResolution(const Sphere& s1, const Sphere& s2) {
	// 距離の計算
	Vector3 distance = s1.center - s2.center;
	// 合計半径の計算
	float totalRaduis = s1.radius + s2.radius;
	// 中心間の距離を計算
	float dist = distance.Length();
	// 重なりを計算
	float penetration = totalRaduis - dist;
	// 方向を計算
	Vector3 direction = Normalize(distance);
	// 押し戻すベクトルを返す
	return direction * penetration;
}

Vector3 PenetrationResolutionAABBandSphere(const AABB& aabb, const Sphere& s1) {
	// 最近接点を求める
	Vector3 closeet = Vector3(
		std::max(aabb.min.x, std::min(s1.center.x, aabb.max.x)),
		std::max(aabb.min.y, std::min(s1.center.y, aabb.max.y)),
		std::max(aabb.min.z, std::min(s1.center.z, aabb.max.z))
	);

	// 中心の距離を計算
	Vector3 delta = s1.center - closeet;
	float dist = delta.Length();
	Vector3 correction = CVector3::ZERO;
	if (dist < s1.radius) {
		float penetration = s1.radius - dist;

		Vector3 direction;
		if (dist > 0.0001f) {
			direction = Normalize(delta); // 通常の方向
		} else {
			direction = Vector3(0, 1, 0); // 完全に中にいるときの安全な方向
		}

		correction = direction * penetration;
	}
	return correction;
}

//================================================================================================//
//								当たり判定の呼び出し関数群											　//
//================================================================================================//

Vector3 PenetrationResolution(const Sphere& s, const AABB& aabb) {
	return PenetrationResolutionAABBandSphere(aabb, s);
}

Vector3 PenetrationResolution(const AABB& aabb, const Sphere& s) {
	return PenetrationResolutionAABBandSphere(aabb, s);
}

Vector3 PenetrationResolution(const std::variant<Sphere, AABB, OBB>& shape1,
						   const std::variant<Sphere, AABB, OBB>& shape2) {
	Vector3 pushbackVector = std::visit(
		[](const auto& lhs, const auto& rhs) {
			return PenetrationResolution(lhs, rhs); // 各組み合わせの CheckCollision を呼び出す
		},
		shape1, shape2);

	return pushbackVector; 
}
