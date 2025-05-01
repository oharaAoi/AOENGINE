#pragma once
#include <cmath>
#include <algorithm>
#include "Engine/Lib/Math/Matrix3x3.h"

class Vector2 final {
public:
	float x;
	float y;

	// コンストラクタ
	//Vector2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

	// 加算
	Vector2 operator+(const Vector2& obj) const { return Vector2(x + obj.x, y + obj.y); }
	Vector2 operator+(float obj) const { return Vector2(x + obj, y + obj); }

	// 加算代入
	Vector2& operator+=(const Vector2& obj) {
		x += obj.x;
		y += obj.y;
		return *this;
	}
	Vector2& operator+=(float obj) {
		x += obj;
		y += obj;
		return *this;
	}

	// 減算
	Vector2 operator-(const Vector2& obj) const { return Vector2(x - obj.x, y - obj.y); }
	Vector2 operator-(float obj) const { return Vector2(x - obj, y - obj); }

	// 減算代入
	Vector2& operator-=(const Vector2& obj) {
		x -= obj.x;
		y -= obj.y;
		return *this;
	}
	Vector2& operator-=(float obj) {
		x -= obj;
		y -= obj;
		return *this;
	}

	// 乗算
	Vector2 operator*(const Vector2& obj) const { return Vector2(x * obj.x, y * obj.y); }
	Vector2 operator*(float obj) const { return Vector2(x * obj, y * obj); }

	// 乗算代入
	Vector2& operator*=(const Vector2& obj) {
		x *= obj.x;
		y *= obj.y;
		return *this;
	}
	Vector2& operator*=(float obj) {
		x *= obj;
		y *= obj;
		return *this;
	}

	// =============================================
	// 数学用関数
	// =============================================

	/// <summary>
	/// ベクトルの長さ
	/// </summary>
	/// <returns></returns>
	float Length() const;

	/// <summary>
	/// 正規化
	/// </summary>
	/// <returns></returns>
	Vector2 Normalize() const;

	/// <summary>
	/// 範囲内に収める
	/// </summary>
	/// <param name="min"></param>
	/// <param name="max"></param>
	/// <returns></returns>
	void Clamp(const Vector2& min, const Vector2& max);

	/// <summary>
	/// 内積
	/// </summary>
	/// <param name="v1">: ベクトル1</param>
	/// <param name="v2">: ベクトル2</param>
	/// <returns></returns>
	static float Dot(const Vector2& v1, const Vector2& v2);

	/// <summary>
	/// 外積
	/// </summary>
	/// <param name="v1">: ベクトル1</param>
	/// <param name="v2">: ベクトル2</param>
	/// <returns></returns>
	static float Cross(const Vector2& v1, const Vector2& v2);

	/// <summary>
	/// 線形補完
	/// </summary>
	/// <param name="st">: 開始位置</param>
	/// <param name="end">: 終了位置</param>
	/// <param name="t"></param>
	/// <returns></returns>
	static Vector2 Lerp(const Vector2& st, const Vector2& end, float t);

	/*拡縮行列*/
	Matrix3x3 MakeScaleMat(const Vector2& scale);

	/*回転行列*/
	Matrix3x3 MakeRotateMat(float theta);

	/*平行移動行列*/
	Matrix3x3 MakeTranslateMat(const Vector2& translate);

	/*アフィン*/
	Matrix3x3 MakeAffine(const Vector2& scale, float theta, const Vector2& translate);
};

/// <summary>
/// Vector2定数
/// </summary>
namespace CVector2 {
	constexpr Vector2 RIGHT	{ 1.0f, 0.0f };
	constexpr Vector2 UP	{ 0.0f, 1.0f };
	constexpr Vector2 ZERO	{ 0.0f, 0.0f };
	constexpr Vector2 UNIT	{ 1.0f, 1.0f };
}