#include "CollisionFunctions.h"
#include "Engine/Lib/Math/MyMath.h"

using namespace Math;

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　球同士の当たり判定
//////////////////////////////////////////////////////////////////////////////////////////////////

bool CheckCollision(const Sphere& s1, const Sphere& s2) {
	// 中心点間の距離を求める
	float distance = Length(s1.center - s2.center);
	// 半径の合計よりも短ければ衝突
	if (distance <= s1.radius + s2.radius) {
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　OBB同士の当たり判定
//////////////////////////////////////////////////////////////////////////////////////////////////

bool CheckCollison(const OBB& colliderA, const OBB& colliderB) {
	// 分離軸の組み合わせを取得
	std::vector<Math::Vector3> crossSeparatingAxises;
	for (uint8_t obb1Index = 0; obb1Index < 3; obb1Index++) {
		for (uint8_t obb2Index = 0; obb2Index < 3; obb2Index++) {
			Math::Vector3 crossAxis = Cross(colliderA.orientations[obb1Index], colliderB.orientations[obb2Index]);
			if (Length(crossAxis) > kEpsilon) { // 長さが非常に小さい場合を除外
				Normalize(crossAxis);
				crossSeparatingAxises.push_back(crossAxis);
			}
		}
	}

	// 面法線をまとめる
	std::vector<Math::Vector3> obb1SurfaceNormals;
	for (uint8_t obbIndex = 0; obbIndex < 3; obbIndex++) {
		obb1SurfaceNormals.push_back(Normalize(TransformNormal(colliderA.orientations[obbIndex], colliderA.matRotate)));
	}

	std::vector<Math::Vector3> obb2SurfaceNormals;
	for (uint8_t obbIndex = 0; obbIndex < 3; obbIndex++) {
		obb2SurfaceNormals.push_back(Normalize(TransformNormal(colliderB.orientations[obbIndex], colliderB.matRotate)));
	}

	// ------------------------------------------------------------
	// 分離軸を割り出す
	std::vector<Math::Vector3> separatingAxises;
	separatingAxises.insert(separatingAxises.end(), obb1SurfaceNormals.begin(), obb1SurfaceNormals.end());
	separatingAxises.insert(separatingAxises.end(), obb2SurfaceNormals.begin(), obb2SurfaceNormals.end());
	separatingAxises.insert(separatingAxises.end(), crossSeparatingAxises.begin(), crossSeparatingAxises.end());

	// obbから頂点を取り出す
	std::vector<Math::Vector3> obb1Indecies = colliderA.MakeIndex();
	std::vector<Math::Vector3> obb2Indecies = colliderB.MakeIndex();

	// 頂点を分離軸候補に射影したベクトルを格納する

	// 取り出した頂点を分離軸へ射影する
	for (uint8_t axis = 0; axis < separatingAxises.size(); axis++) {
		std::vector<float> obb1ProjectIndecies;
		std::vector<float> obb2ProjectIndecies;

		for (uint8_t oi = 0; oi < obb1Indecies.size(); oi++) {
			// 各obbの頂点を射影する
			// 正射影ベクトルの長さを求める
			obb1ProjectIndecies.push_back(Dot(obb1Indecies[oi], separatingAxises[axis]));
			obb2ProjectIndecies.push_back(Dot(obb2Indecies[oi], separatingAxises[axis]));
		}

		// 最大値/最小値を取り出す
		float maxObb1 = *std::max_element(obb1ProjectIndecies.begin(), obb1ProjectIndecies.end());
		float maxObb2 = *std::max_element(obb2ProjectIndecies.begin(), obb2ProjectIndecies.end());
		float minObb1 = *std::min_element(obb1ProjectIndecies.begin(), obb1ProjectIndecies.end());
		float minObb2 = *std::min_element(obb2ProjectIndecies.begin(), obb2ProjectIndecies.end());

		// 影の長さを得る
		float projectLenght1 = float(maxObb1 - minObb1);
		float projectLenght2 = float(maxObb2 - minObb2);

		float sumSpan = projectLenght1 + projectLenght2;
		float longSpan = ((std::max)(maxObb1, maxObb2) - ((std::min)(minObb1, minObb2)));

		// 影の長さの合計 < 2つの影の両端の差分だったら分離軸が存在しているためfalse
		if (sumSpan <= longSpan) {
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　AABBとAABB
//////////////////////////////////////////////////////////////////////////////////////////////////

bool CheckCollision(const AABB& aabb1, const AABB& aabb2) {
	if (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) {

	} else {
		return false;
	}

	if (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) {

	} else {
		return false;
	}

	if (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z) {

	} else {
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　AABBと球の当たり判定
//////////////////////////////////////////////////////////////////////////////////////////////////

bool CheckCollisionAABBandSphere(const AABB& aabb, const Sphere& sphere) {
	// 最近傍点を求める
	Math::Vector3 closestPoint{
		std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
		std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
		std::clamp(sphere.center.z, aabb.min.z, aabb.max.z)
	};

	// 最近傍点ト球の中心の距離を求める
	float distance = Length(closestPoint - sphere.center);

	// 距離が半径よりも小さければ衝突
	if (distance <= sphere.radius) {
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　OBBと球の当たり判定
//////////////////////////////////////////////////////////////////////////////////////////////////

bool CheckCollisionOBBandSphere(const OBB& obb, const Sphere& sphere) {
	// 回転行列を作成する
	Math::Matrix4x4 rotateMatrix = obb.matRotate;
	// 平行移動分を作成
	Math::Matrix4x4 matTranslate = obb.center.MakeTranslateMat();
	// ワールド行列を作成
	Math::Matrix4x4 obbMatWorld = rotateMatrix * matTranslate;
	Math::Matrix4x4 obbMatWorldInverse = obbMatWorld.Inverse();

	// 中心点を作成
	Math::Vector3 centerInOBBLocal = Transform(sphere.center, obbMatWorldInverse);

	// OBBからABBを作成
	AABB aabbOBBLocal{ .min = obb.size * -1, .max = obb.size };
	Sphere sphereOBBLocal{ .center = centerInOBBLocal, .radius = sphere.radius};

	// ローカル空間で衝突判定
	if (CheckCollisionAABBandSphere(aabbOBBLocal, sphereOBBLocal)) {
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　OBBとAABBの当たり判定
//////////////////////////////////////////////////////////////////////////////////////////////////

bool CheckCollisionAABBandOBB(const OBB& obb, const AABB& aabb) {
	AABB aabbOBBLocal{ .min = obb.size * -1, .max = obb.size };
	if (CheckCollision(aabbOBBLocal, aabb)) {
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　SphereとLineの当たり判定
//////////////////////////////////////////////////////////////////////////////////////////////////

bool CheckCollisionSphereAndLine(const Sphere& sphere, const Line& line) {
	Math::Vector3 A = line.origin;
	Math::Vector3 B = line.origin + line.diff;

	Math::Vector3 segment = B - A;
	float segLen2 = Math::Vector3::Dot(segment, segment);

	// 退避：長さ0の線分は origin との距離判定にする
	if (segLen2 == 0.0f) {
		float dist2 = (A - sphere.center).LengthSquared();
		return dist2 <= sphere.radius * sphere.radius;
	}

	Math::Vector3 AC = sphere.center - A;

	// 射影
	float t = Math::Vector3::Dot(AC, segment) / segLen2;
	t = std::clamp(t, 0.0f, 1.0f);

	// 線分上の最接近点
	Math::Vector3 P = A + segment * t;

	// 距離²で比較
	float dist2 = (P - sphere.center).LengthSquared();
	return dist2 <= sphere.radius * sphere.radius;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　AABBとLineの当たり判定
//////////////////////////////////////////////////////////////////////////////////////////////////

bool CheckCollisionAABBandLine(const AABB& aabb, const Line& line) {
	// 衝突点の媒介変数を求める
	Math::Vector3 tMin = (aabb.min - line.origin) / line.diff;
	Math::Vector3 tMax = (aabb.max - line.origin) / line.diff;

	if (std::isnan(tMin.x)) { tMin.x = 0; }
	if (std::isnan(tMin.y)) { tMin.y = 0; }
	if (std::isnan(tMin.z)) { tMin.z = 0; }

	if (std::isnan(tMax.x)) { tMax.x = 99; }
	if (std::isnan(tMax.y)) { tMax.y = 99; }
	if (std::isnan(tMax.z)) { tMax.z = 99; }

	// 衝突点の内近い方と小さい方を求める
	Math::Vector3 tNear{
		std::min(tMin.x, tMax.x),
		std::min(tMin.y, tMax.y),
		std::min(tMin.z, tMax.z),
	};

	// 遠い方
	Math::Vector3 tFar{
		std::max(tMin.x, tMax.x),
		std::max(tMin.y, tMax.y),
		std::max(tMin.z, tMax.z),
	};

	// 貫通している状況かを調べる
	// 近い方の大きい方を求める
	float tmin = std::max(std::max(tNear.x, tNear.y), tNear.z);
	// 遠い方の小さい方を求める
	float tmax = std::min(std::min(tFar.x, tFar.y), tFar.z);

	if (tmin <= tmax) {
		if (0 <= tmin && tmin <= 1) {
			return true;
		}

		if (0 <= tmax && tmax <= 1) {
			return true;
		}

		if (tmin <= 0 && tmax >= 1) {
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　OBBとLineの当たり判定
//////////////////////////////////////////////////////////////////////////////////////////////////

bool CheckCollisionOBBandLine(const OBB& obb, const Line& line) {
	// 回転行列を作成する
	Math::Matrix4x4 rotateMatrix = obb.matRotate;
	// 平行移動分を作成
	Math::Matrix4x4 matTranslate = obb.center.MakeTranslateMat();
	// ワールド行列を作成
	Math::Matrix4x4 obbMatWorld = rotateMatrix * matTranslate;
	// -M
	Math::Matrix4x4 obbMatWorldInverse = Inverse(obbMatWorld);

	// 線分の始点と終点をAABBのローカル空間に変換する
	Math::Vector3 localOrigin = Transform(line.origin, obbMatWorldInverse);
	Math::Vector3 localEnd = Transform(line.origin + line.diff, obbMatWorldInverse);

	// OBBからABBを作成
	AABB aabbOBBLocal{ .min = obb.size * -1, .max = obb.size };
	// ローカルの線分を生成
	Line localLine = {
		.origin = localOrigin,
		.diff = localEnd - localOrigin
	};

	// 衝突点の媒介変数を求める
	Math::Vector3 tMin = (aabbOBBLocal.min - line.origin) / line.diff;
	Math::Vector3 tMax = (aabbOBBLocal.max - line.origin) / line.diff;

	if (std::isnan(tMin.x)) { tMin.x = 0; }
	if (std::isnan(tMin.y)) { tMin.y = 0; }
	if (std::isnan(tMin.z)) { tMin.z = 0; }

	if (std::isnan(tMax.x)) { tMax.x = 99; }
	if (std::isnan(tMax.y)) { tMax.y = 99; }
	if (std::isnan(tMax.z)) { tMax.z = 99; }

	// 衝突点の内近い方と小さい方を求める
	Math::Vector3 tNear{
		std::min(tMin.x, tMax.x),
		std::min(tMin.y, tMax.y),
		std::min(tMin.z, tMax.z),
	};

	// 遠い方
	Math::Vector3 tFar{
		std::max(tMin.x, tMax.x),
		std::max(tMin.y, tMax.y),
		std::max(tMin.z, tMax.z),
	};

	// 貫通している状況かを調べる
	// 近い方の大きい方を求める
	float tmin = std::max(std::max(tNear.x, tNear.y), tNear.z);
	// 遠い方の小さい方を求める
	float tmax = std::min(std::min(tFar.x, tFar.y), tFar.z);

	if (tmin <= tmax) {
		return true;
	}

	return false;
}

//================================================================================================//
//								当たり判定の呼び出し関数群											　//
//================================================================================================//

bool CheckCollision(const AABB& aabb, const Sphere& sphere) {
	return CheckCollisionAABBandSphere(aabb, sphere);
}
bool CheckCollision(const Sphere& sphere, const AABB& aabb) {
	return CheckCollisionAABBandSphere(aabb, sphere);
}

bool CheckCollision(const OBB& obb, const Sphere& sphere) {
	return CheckCollisionOBBandSphere(obb, sphere);
}
bool CheckCollision(const Sphere& sphere, const OBB& obb) {
	return CheckCollisionOBBandSphere(obb, sphere);
}

bool CheckCollision(const OBB& obb, const AABB& aabb) {
	return CheckCollisionAABBandOBB(obb, aabb);
}
bool CheckCollision(const AABB& aabb, const OBB& obb) {
	return CheckCollisionAABBandOBB(obb, aabb);
}

bool CheckCollision(const Sphere& sphere, const Line& line) {
	return CheckCollisionSphereAndLine(sphere, line);
}
bool CheckCollision(const AABB& aabb, const Line& line) {
	return CheckCollisionAABBandLine(aabb, line);
}
bool CheckCollision(const OBB& obb, const Line& line) {
	return CheckCollisionOBBandLine(obb, line);
}


bool CheckCollision(const std::variant<Sphere, AABB, OBB, Line>& shape1, const std::variant<Sphere, AABB, OBB, Line>& shape2) {
	return std::visit(
		[](const auto& lhs, const auto& rhs) {
			return CheckCollision(lhs, rhs); // 各組み合わせの CheckCollision を呼び出す
		},
		shape1, shape2);
}
