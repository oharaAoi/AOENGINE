#include "PenetrationResolution.h"
#include "Engine/Lib/Math/MyMath.h"

Math::Vector3 PenetrationResolution(const Math::Sphere& s1, const Math::Sphere& s2) {
	// 距離の計算
	Math::Vector3 distance = s1.center - s2.center;
	// 合計半径の計算
	float totalRaduis = s1.radius + s2.radius;
	// 中心間の距離を計算
	float dist = distance.Length();
	// 重なりを計算
	float penetration = totalRaduis - dist;
	// 方向を計算
	Math::Vector3 direction = Normalize(distance);
	// 押し戻すベクトルを返す
	return direction * penetration;
}

Math::Vector3 PenetrationResolutionAABBandSphere(const Math::AABB& aabb, const Math::Sphere& s1) {
	// 最近接点を求める
	Math::Vector3 closeet = Math::Vector3(
		std::max(aabb.min.x, std::min(s1.center.x, aabb.max.x)),
		std::max(aabb.min.y, std::min(s1.center.y, aabb.max.y)),
		std::max(aabb.min.z, std::min(s1.center.z, aabb.max.z))
	);

	// 中心の距離を計算
	Math::Vector3 delta = s1.center - closeet;
	float dist = delta.Length();
	Math::Vector3 correction = CVector3::ZERO;
	if (dist < s1.radius) {
		float penetration = s1.radius - dist;

		Math::Vector3 direction;
		if (dist > 0.0001f) {
			direction = Normalize(delta); // 通常の方向
		} else {
			direction = Math::Vector3(0, 1, 0); // 完全に中にいるときの安全な方向
		}

		correction = direction * penetration;
	}
	return correction;
}

Math::Vector3 PenetrationResolutionAABBandAABB(const Math::AABB& aabb1, const Math::AABB& aabb2) {
	// 押し戻し量を計算する
	float overlapX = std::min(aabb1.max.x, aabb2.max.x) - std::max(aabb1.min.x, aabb2.min.x);
	float overlapY = std::min(aabb1.max.y, aabb2.max.y) - std::max(aabb1.min.y, aabb2.min.y);
	float overlapZ = std::min(aabb1.max.z, aabb2.max.z) - std::max(aabb1.min.z, aabb2.min.z);

	// オーバーラップが最小の軸を探す
	float minOverlap = overlapX;
	Math::Vector3 mtvDir(1.0f, 0.0f, 0.0f);

	if (overlapY < minOverlap) {
		minOverlap = overlapY;
		mtvDir = Math::Vector3(0.0f, 1.0f, 0.0f);
	}

	if (overlapZ < minOverlap) {
		minOverlap = overlapZ;
		mtvDir = Math::Vector3(0.0f, 0.0f, 1.0f);
	}

	return mtvDir * minOverlap;
}

//================================================================================================//
//								当たり判定の呼び出し関数群											　//
//================================================================================================//

Math::Vector3 PenetrationResolution(const Math::Sphere& s, const Math::AABB& aabb) {
	return PenetrationResolutionAABBandSphere(aabb, s);
}

Math::Vector3 PenetrationResolution(const Math::AABB& aabb, const Math::Sphere& s) {
	return PenetrationResolutionAABBandSphere(aabb, s);
}

Math::Vector3 PenetrationResolution(const Math::AABB& aabb1, const Math::AABB& aabb2) {
	return PenetrationResolutionAABBandAABB(aabb1, aabb2);
}

Math::Vector3 PenetrationResolution(const std::variant<Math::Sphere, Math::AABB, Math::OBB, Math::Line>& shape1,
						   const std::variant<Math::Sphere, Math::AABB, Math::OBB, Math::Line>& shape2) {
	Math::Vector3 pushbackVector = CVector3::ZERO;
	pushbackVector = std::visit(
		[](const auto& lhs, const auto& rhs) {
			return PenetrationResolution(lhs, rhs); // 各組み合わせの CheckCollision を呼び出す
		},
		shape1, shape2);

	return pushbackVector; 
}
