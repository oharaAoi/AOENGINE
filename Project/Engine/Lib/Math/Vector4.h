#pragma once
#include "Engine/Lib/Math/Matrix4x4.h"
#include "Engine/Lib/Math/Vector3.h"

namespace Math {

class Vector4 final {
public:

	float x;
	float y;
	float z;
	float w;

	Vector4() = default;
	~Vector4() = default;

	Math::Vector4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w) {
	}

	Math::Vector4(const Math::Vector3& vec3, float w = 1.0f)
		: x(vec3.x), y(vec3.y), z(vec3.z), w(w) {
	}

	// =============================================
	/// <summary>
	/// 加算
	/// </summary>
	/// <param name="obj"></param>
	/// <returns></returns>
	Math::Vector4 operator+(const Math::Vector4& obj) const { return Math::Vector4(x + obj.x, y + obj.y, z + obj.z, w + obj.w); }

	Math::Vector4 operator+(const float& obj) const { return Math::Vector4(x + obj, y + obj, z + obj, w + obj); }

	Math::Vector4 operator+=(const Math::Vector4& obj) {
		x += obj.x;
		y += obj.y;
		z += obj.z;
		w += obj.w;
		return *this;
	}

	Math::Vector4 operator+=(const float& obj) {
		x += obj;
		y += obj;
		z += obj;
		w += obj;
		return *this;
	}

	/// <summary>
	/// 減算
	/// </summary>
	/// <param name="obj"></param>
	/// <returns></returns>
	Math::Vector4 operator-(const Math::Vector4& obj) const { return Math::Vector4(x - obj.x, y - obj.y, z - obj.z, w - obj.w); }

	Math::Vector4 operator-(const float& obj) const { return Math::Vector4(x - obj, y - obj, z - obj, w - obj); }

	Math::Vector4 operator-=(const Math::Vector4& obj) {
		x -= obj.x;
		y -= obj.y;
		z -= obj.z;
		w -= obj.w;
		return *this;
	}

	Math::Vector4 operator-=(const float& obj) {
		x -= obj;
		y -= obj;
		z -= obj;
		w -= obj;
		return *this;
	}

	/// <summary>
	/// 乗算
	/// </summary>
	/// <param name="obj"></param>
	/// <returns></returns>
	Math::Vector4 operator*(const Math::Vector4& obj) const { return Math::Vector4(x * obj.x, y * obj.y, z * obj.z, w * obj.w); }

	Math::Vector4 operator*(const float& obj) const { return Math::Vector4(x * obj, y * obj, z * obj, w * obj); }

	Math::Vector4 operator*=(const Math::Vector4& obj) {
		x *= obj.x;
		y *= obj.y;
		z *= obj.z;
		w *= obj.w;
		return *this;
	}

	Math::Vector4 operator*=(const float& obj) {
		x *= obj;
		y *= obj;
		z *= obj;
		w *= obj;
		return *this;
	}

	// Matrix
	Math::Vector4 operator*(const Math::Matrix4x4& mat) {
		Math::Vector4 result{};
		result.x = mat.m[0][0] * x + mat.m[0][1] * y + mat.m[0][2] * z + mat.m[0][4];
		result.y = mat.m[1][0] * x + mat.m[1][1] * y + mat.m[1][2] * z + mat.m[1][4];
		result.z = mat.m[2][0] * x + mat.m[2][1] * y + mat.m[2][2] * z + mat.m[2][4];
		//result.w = mat.m[3][0] * x + mat.m[3][1] * y + mat.m[3][2] * z + mat.m[3][4];
		return result;
	}

	static Math::Vector4 Lerp(const Math::Vector4& start, const Math::Vector4& end, float t);
};

}