#pragma once
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Math/MyMatrix.h"
#include <cmath>
#define NOMINMAX
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <numbers>
#include <limits>

////////////////////////////////////////////////////////////////////////////////////
// 数学用定数
////////////////////////////////////////////////////////////////////////////////////

constexpr const float kPI = std::numbers::pi_v<float>;
constexpr const float kPI2 = kPI * 2.0f;
constexpr const float kHPI = kPI * 0.5f;

constexpr const float kToRadian = std::numbers::pi_v<float> / 180.0f;
constexpr const float kToDegree = 180.0f / std::numbers::pi_v<float>;

constexpr const float kGravity = -9.81f;

constexpr float kInf = std::numeric_limits<float>::infinity();

constexpr const float kEpsilon = std::numeric_limits<float>::epsilon();

////////////////////////////////////////////////////////////////////////////////////
// 基礎関数
////////////////////////////////////////////////////////////////////////////////////

Vector3 DegToRad(const Vector3& rotate);
Vector3 RadToDeg(const Vector3& rotate);

float Clamp01(float value);

float Distance(const Vector3& v1, const Vector3& v2);

/// <summary>
/// 長さを計算する
/// </summary>
/// <param name="vec3"></param>
/// <returns></returns>
float Length(const Vector3& vec3);

/// <summary>
/// 正規化
/// </summary>
/// <param name="value"></param>
/// <param name="min"></param>
/// <param name="max"></param>
/// <returns></returns>
float Normalize(float value, float min, float max);

/// <summary>
/// 正規化
/// </summary>
/// <param name="vec3"></param>
/// <returns></returns>
Vector3 Normalize(const Vector3& vec3);

/// <summary>
/// 内積
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
float Dot(const Vector3& v1, const Vector3& v2);

/// <summary>
/// クロス積
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
Vector3 Cross(const Vector3& v1, const Vector3& v2);

/// <summary>
/// スクリーン座標からワールド座標に変換する関数
/// </summary>
/// <param name="screenPos">スクリーン上の座標</param>
/// <param name="inverseWvpMat">逆ワールドプロジェクション行列</param>
/// <param name="setDirection">ワールド上のどこに設定するか</param>
/// <returns>ワールド座標</returns>
Vector3 ScreenToWorldCoordinate(const Vector2& screenPos, const Matrix4x4& inverseWvpMat, const float& setDirection);

/// <summary>
/// ワールド座標系からスクリーン座標系の座標を取得する関数
/// </summary>
/// <param name="targetWorldMat">: 対象のMatrix</param>
/// <param name="vpvpMat">: viewProjection * viewPort </param>
/// <returns>スクリーン座標</returns>
Vector2 WorldToScreenCoordinate(const Matrix4x4& targetWorldMat, const Matrix4x4& vpvpMat);

/// <summary>
/// ベクトル変換
/// </summary>
/// <param name="v"></param>
/// <param name="m"></param>
/// <returns></returns>
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

////////////////////////////////////////////////////////////////////////////////////
// 数字を求めたり
////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 引数の桁数を求める
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="value"></param>
/// <returns></returns>
template <typename T>
T DegitCount(T value) {
	if (value == 0.0f) { return 0; }
	// 桁数 = log10(値) + 1(std::log10はvalueが10以下の時は0が返される)
	return static_cast<T>(std::floor(std::log10(value)) + 1);
}

/// <summary>
/// 引数の値から任意の桁目の数値を取り出す
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="value">: 値</param>
/// <param name="n">: 任意の桁目</param>
/// <returns></returns>
template <typename T>
T IntegerCount(T value, int n) {
	if (n == 0) {
		assert("not 0 Interger");
	}
	T num = (static_cast<int>(value) / static_cast<int>(std::pow(10, n - 1)));
	T result = num % 10;
	return result;
}

/// <summary>
/// 数値のTextureから値に対応する左上座標の位置を求める
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="value"></param>
/// <param name="size"></param>
/// <returns></returns>
template <typename T>
Vector2 NumberSpriteLt(T value, const Vector2& size) {
	if (value == 0) {
		return { 0.0f, 0.0f };
	} else if (value == 1) {
		return { size.x , 0.0f };
	} else if (value == 2) {
		return { size.x * 2.0f, 0.0f };
	} else if (value == 3) {
		return { size.x * 3.0f , 0.0f };
	} else if (value == 4) {
		return { size.x * 4.0f , 0.0f };
	} else if (value == 5) {
		return { size.x * 5.0f, 0.0f };
	} else if (value == 6) {
		return { size.x * 6.0f, 0.0f };
	} else if (value == 7) {
		return { size.x * 7.0f, 0.0f };
	} else if (value == 8) {
		return { size.x * 8.0f, 0.0f };
	} else {
		return { size.x * 9.0f , 0.0f };
	}
}

////////////////////////////////////////////////////////////////////////////////////
// 線形補完
////////////////////////////////////////////////////////////////////////////////////

float Lerp(float v1, float v2, float t);
Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t);
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t);

/// <summary>
/// 線形補完
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <param name="t"></param>
/// <returns></returns>
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);

/// <summary>
/// 最短角度を求める
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="t"></param>
/// <returns></returns>
float LerpShortAngle(const float& originalAngle, const float& targetAngle, const float& t);

/// <summary>/// 
/// 角度を求める
/// </summary>///
///  <param name="v1"></param>/// 
/// <param name="v2"></param>///
///  <returns></returns>
float Angle(const Vector3& v1, const Vector3& v2);

/// <summary>
/// 臨界減衰スプリング
/// </summary>
/// <param name="current"></param>
/// <param name="target"></param>
/// <param name="currentVelocity"></param>
/// <param name="smoothTime"></param>
/// <param name="maxSpped"></param>
/// <param name="deltaTime"></param>
/// <returns></returns>
float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);

Vector3 SmoothDamp(const Vector3& current, const Vector3& target, Vector3& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);

/// <summary>/// 
/// CatmullRom補完
/// </summary>/// 
/// <param name="p0">点0の座標</param>/// 
/// <param name="p1">点1の座標</param>/// 
/// <param name="p2">点2の座標</param>/// 
/// <param name="p3">点3の座標</param>/// 
/// <param name="t">点1と点2の間で指定された座標</param>/// 
/// <returns>点1と点2の間で指定された座標</returns>
Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);

/// <summary>/// 
/// CatmullRomスプライン曲線上の座標を得る
/// </summary>///
/// <param name="points">制御点の集合</param>/// 
/// <param name="t">スプライン全区間の中で割合指定</param>/// 
/// <returns>座標</returns>
Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t);

/// <summary>
/// ベジエ曲線を書く
/// </summary>
///  <param name="controlPoint">制御点がまとまった配列</param>
///  <returns></returns>
Vector3 Bezier(const std::vector<Vector3>& controlPoint, const float& t);
//
///// <summary>
///// ベジエ曲線を書く
///// </summary>
///// <param name="controlPoint">制御点がまとまった配列</param>
///// <returns></returns>
//Vector3 Bezier(const Vector3& v1, const Vector3& v2, const Vector3& v3, const float& t);

////////////////////////////////////////////////////////////////////////////////////
// 変換
////////////////////////////////////////////////////////////////////////////////////

template <typename T>
	requires requires (T v) { v.x; v.y; }
std::string ToString(const T& v) {
	std::ostringstream oss;
	oss << "(" << v.x << ", " << v.y;

	if constexpr (requires { v.z; }) {
		oss << ", " << v.z;
	}
	if constexpr (requires { v.w; }) {
		oss << ", " << v.w;
	}

	oss << ")";
	return oss.str();
}


uint32_t Red(uint32_t color);
uint32_t Green(uint32_t color);
uint32_t Blue(uint32_t color);
uint32_t Alpha(uint32_t color);
Vector4 FloatColor(uint32_t color);
uint32_t IntColor(const Vector4& color);
uint32_t HSV_to_RGB(float h, float s, float v, float alpha);