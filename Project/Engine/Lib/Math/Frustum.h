#pragma once
#include <array>
#include "Engine/Lib/Math/MathStructures.h"

namespace Math {

/// <summary>
/// 3D空間上の平面を表す構造体。
/// Frustum Cullingでは各クリップ平面との距離判定に使用します。
/// </summary>
struct Plane {
	Math::Vector3 normal = CVector3::ZERO;
	float distance = 0.0f;

	/// <summary>
	/// 点が平面のどちら側にあるかを符号付き距離で返します。
	/// </summary>
	float SignedDistance(const Math::Vector3& point) const;

	/// <summary>
	/// 法線の長さを1に揃え、距離判定に使える状態へ正規化します。
	/// </summary>
	void Normalize();
};

/// <summary>
/// カメラの視錐台を表すクラス。
/// モデルの境界球と交差判定し、画面外オブジェクトの描画を省くために使用します。
/// </summary>
class Frustum final {
public:
	/// <summary>
	/// 視錐台を構成する6つの平面番号。
	/// </summary>
	enum PlaneIndex {
		Left,
		Right,
		Bottom,
		Top,
		Near,
		Far,
		Count
	};

	/// <summary>
	/// ViewProjection行列から視錐台の6平面を作成します。
	/// </summary>
	static Math::Frustum FromViewProjection(const Math::Matrix4x4& viewProjection);

	/// <summary>
	/// 境界球が視錐台と交差しているかを返します。
	/// </summary>
	bool Intersects(const Math::Sphere& sphere) const;

	/// <summary>
	/// デバッグ表示や追加判定用に平面配列を取得します。
	/// </summary>
	const std::array<Math::Plane, Count>& GetPlanes() const { return planes_; }

private:
	std::array<Math::Plane, Count> planes_{};
};

}
