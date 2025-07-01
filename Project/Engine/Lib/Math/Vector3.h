#pragma once
#include <cmath>
#include <algorithm>
#include "Matrix4x4.h"

/// <summary>
/// 3次元ベクトル
/// </summary>
class Vector3 final {
public:

	float x;
	float y;
	float z;

	// =============================================
	/// <summary>
	/// 加算
	/// </summary>
	/// <param name="obj"></param>
	/// <returns></returns>
	Vector3 operator+(const Vector3& obj) const { return Vector3(x + obj.x, y + obj.y, z + obj.z); }

	Vector3 operator+(const float& obj) const { return Vector3(x + obj, y + obj, z + obj); }

	Vector3 operator+=(const Vector3& obj) {
		x += obj.x;
		y += obj.y;
		z += obj.z;
		return *this;
	}

	Vector3 operator+=(const float& obj) {
		x += obj;
		y += obj;
		z += obj;
		return *this;
	}

	/// <summary>
	/// 減算
	/// </summary>
	/// <param name="obj"></param>
	/// <returns></returns>
	Vector3 operator-(const Vector3& obj) const { return Vector3(x - obj.x, y - obj.y, z - obj.z); }

	Vector3 operator-(const float& obj) const { return Vector3(x - obj, y - obj, z - obj); }

	Vector3 operator-=(const Vector3& obj) {
		x -= obj.x;
		y -= obj.y;
		z -= obj.z;
		return *this;
	}

	Vector3 operator-=(const float& obj) {
		x -= obj;
		y -= obj;
		z -= obj;
		return *this;
	}

	/// <summary>
	/// 乗算
	/// </summary>
	/// <param name="obj"></param>
	/// <returns></returns>
	Vector3 operator*(const Vector3& obj) const { return Vector3(x * obj.x, y * obj.y, z * obj.z); }

	// メンバー関数としてのスカラー乗算 (Vector3 * float)
	[[nodiscard]] constexpr Vector3 operator*(float scalar) const {
		return Vector3(x * scalar, y * scalar, z * scalar);
	}

	// フリー関数としてのスカラー乗算 (float * Vector3)
	[[nodiscard]] friend constexpr Vector3 operator*(float scalar, const Vector3& vec) {
		return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
	}


	Vector3 operator*=(const Vector3& obj) {
		x *= obj.x;
		y *= obj.y;
		z *= obj.z;
		return *this;
	}

	Vector3 operator*=(const float& obj) {
		x *= obj;
		y *= obj;
		z *= obj;
		return *this;
	}

	Vector3 operator/(const float& obj) {
		return{
		x / obj,
		y / obj,
		z / obj
		};
	}

	// Matrix
	Vector3 operator*(const Matrix4x4& mat) {
		Vector3 result{};

		result.x = mat.m[0][0] * x + mat.m[1][0] * y + mat.m[2][0] * z + mat.m[3][0];
		result.y = mat.m[0][1] * x + mat.m[1][1] * y + mat.m[2][1] * z + mat.m[3][1];
		result.z = mat.m[0][2] * x + mat.m[1][2] * y + mat.m[2][2] * z + mat.m[3][2];

		return result;
	}

	/// <summary>
	/// 正規化
	/// </summary>
	/// <returns></returns>
	Vector3 Normalize() const;

	/// <summary>
	/// 長さをとる
	/// </summary>
	/// <returns></returns>
	float Length() const;

	/// <summary>
	/// 範囲内に収める
	/// </summary>
	/// <param name="min"></param>
	/// <param name="max"></param>
	/// <returns></returns>
	void Clamp(const Vector3& min, const Vector3& max);

	/// <summary>
	/// 拡縮行列の作成
	/// </summary>
	/// <returns></returns>
	Matrix4x4 MakeScaleMat() const;

	/// <summary>
	/// 回転行列の作成
	/// </summary>
	/// <returns></returns>
	Matrix4x4 MakeRotateMat() const;

	/// <summary>
	/// 平行行列の作成
	/// </summary>
	/// <returns></returns>
	Matrix4x4 MakeTranslateMat() const;

	/// <summary>
	/// 内積
	/// </summary>
	/// <param name="v1">: ベクトル1</param>
	/// <param name="v2">: ベクトル2</param>
	/// <returns></returns>
	static float Dot(const Vector3& v1, const Vector3& v2);

	/// <summary>
	/// 外積
	/// </summary>
	/// <param name="v1">: ベクトル1</param>
	/// <param name="v2">: ベクトル2</param>
	/// <returns></returns>
	static Vector3 Cross(const Vector3& v1, const Vector3& v2);

	/// <summary>
	/// 線形補完
	/// </summary>
	/// <param name="start">: 開始</param>
	/// <param name="end">: 終わり</param>
	/// <param name="t">: 補完係数</param>
	/// <returns></returns>
	static Vector3 Lerp(const Vector3& start, const Vector3& end, float t);

	static Vector3 Min(const Vector3& min, const Vector3& target);
	static Vector3 Max(const Vector3& max, const Vector3& target);

	/// <summary>
	/// Matrixを作成する(z軸)
	/// </summary>
	/// <param name="z"></param>
	/// <returns></returns>
	Matrix4x4 MakeRollMatrix(float z_Roll) const;

	/// <summary>
	/// Matrixを作成する(x軸)
	/// </summary>
	/// <param name="x"></param>
	/// <returns></returns>
	Matrix4x4 MakePitchMatrix(float x_Picth) const;

	/// <summary>
	/// Matrixを作成する(y軸)
	/// </summary>
	/// <param name="y"></param>
	/// <returns></returns>
	Matrix4x4 MakeYawMatrix(float y_Yaw) const;
};

// フリー関数
//inline Vector3 operator*(const float& obj, const Vector3& vec) {
//	return Vector3(vec.x * obj, vec.y * obj, vec.z * obj);
//}

/// <summary>
/// Vector3定数
/// </summary>
namespace CVector3 {
	constexpr Vector3 RIGHT		{ 1.0f, 0.0f, 0.0f };
	constexpr Vector3 UP		{ 0.0f, 1.0f, 0.0f };
	constexpr Vector3 FORWARD	{ 0.0f ,0.0f, 1.0f };
	constexpr Vector3 ZERO		{ 0.0f, 0.0f, 0.0f };
	constexpr Vector3 UNIT		{ 1.0f, 1.0f, 1.0f };
	constexpr Vector3 INF		{ std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() };
}