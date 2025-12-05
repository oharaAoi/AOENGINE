#pragma once
#include "Engine/Lib/Math/Vector4.h"

/// <summary>
/// 色を変更するクラス
/// </summary>
class Color final {
public:

	constexpr Color(float _r = 0, float _g = 0, float _b = 0, float _a = 1.0f)
		: r(_r), g(_g), b(_b), a(_a) {
	}

	~Color() = default;

	float r, g, b, a;

	static const Color black;
	static const Color white;
	static const Color red;
	static const Color green;
	static const Color blue;

public: // method

	static Color Lerp(const Color& _a, const Color& b, float _t);

	static Color RgbTo01(float _r, float _g, float _b);

public: // operator

	// ------------------- 加算・減算 ------------------- //

	Color operator+(const Color& other) const {
		return Color(r + other.r, g + other.g, b + other.b, a + other.a);
	}
	Color operator-(const Color& other) const {
		return Color(r - other.r, g - other.g, b - other.b, a - other.a);
	}

	Color& operator+=(const Color& other) {
		r += other.r; g += other.g; b += other.b; a += other.a;
		return *this;
	}
	Color& operator-=(const Color& other) {
		r -= other.r; g -= other.g; b -= other.b; a -= other.a;
		return *this;
	}

	// ------------------- 掛け算・割り算 ------------------- //

	Color operator*(const Color& other) const {
		return Color(r * other.r, g * other.g, b * other.b, a * other.a);
	}
	Color operator/(const Color& other) const {
		return Color(r / other.r, g / other.g, b / other.b, a / other.a);
	}
	Color& operator*=(const Color& other) {
		r *= other.r; g *= other.g; b *= other.b; a *= other.a;
		return *this;
	}
	Color& operator/=(const Color& other) {
		r /= other.r; g /= other.g; b /= other.b; a /= other.a;
		return *this;
	}

	// ------------------- スカラーとの演算 ------------------- //

	Color operator*(float s) const {
		return Color(r * s, g * s, b * s, a * s);
	}
	Color operator/(float s) const {
		return Color(r / s, g / s, b / s, a / s);
	}
	Color& operator*=(float s) {
		r *= s; g *= s; b *= s; a *= s;
		return *this;
	}
	Color& operator/=(float s) {
		r /= s; g /= s; b /= s; a /= s;
		return *this;
	}

	operator Math::Vector4() const {
		return Math::Vector4(r, g, b, a);
	}
};

inline constexpr Color Color::black{ 0,0,0,1 };
inline constexpr Color Color::white{ 1,1,1,1 };
inline constexpr Color Color::red{ 1,0,0,1 };
inline constexpr Color Color::green{ 0,1,0,1 };
inline constexpr Color Color::blue{ 0,0,1,1 };