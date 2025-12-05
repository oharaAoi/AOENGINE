#include "Vector2.h"

using namespace Math;

float Math::Vector2::Length() const {
	return std::sqrt((x * x) + (y * y));
}

Math::Vector2 Math::Vector2::Normalize() const {
	Math::Vector2 result = { x, y };
	float len = this->Length();
	if (len != 0) {
		result.x /= len;
		result.y /= len;
	}
	return result;
}

void Math::Vector2::Clamp(const Math::Vector2& min, const Math::Vector2& max) {
	if (min.x > max.x) {
		x = std::clamp(x, min.x, max.x);
	}

	if (min.y > max.y) {
		y = std::clamp(y, min.y, max.y);
	}
}

float Math::Vector2::Dot(const Math::Vector2& v1, const Math::Vector2& v2) {
	return ((v1.x * v2.x) + (v1.y * v2.y));
}

float Math::Vector2::Cross(const Math::Vector2& v1, const Math::Vector2& v2) {
	return (v1.x * v2.y) - (v2.x * v1.y);
}

Math::Vector2 Math::Vector2::Lerp(const Math::Vector2& st, const Math::Vector2& end, float t) {
	return {
		std::lerp(st.x, end.x, t),
		std::lerp(st.y, end.y, t)
	};
}

Math::Matrix3x3 Math::Vector2::MakeScaleMat(const Math::Vector2& scale) {
	Math::Matrix3x3 result{};
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = 1;

	return result;
}

Math::Matrix3x3 Math::Vector2::MakeRotateMat(float theta) {
	Math::Matrix3x3 result{};
	result.m[0][0] = cosf(theta);
	result.m[0][1] = sinf(theta);
	result.m[1][0] = -sinf(theta);
	result.m[1][1] = cosf(theta);
	result.m[2][2] = 1;

	return result;
}

Math::Matrix3x3 Math::Vector2::MakeTranslateMat(const Math::Vector2& translate) {
	Math::Matrix3x3 result{};
	result.m[0][0] = 1;
	result.m[1][1] = 1;
	result.m[2][0] = translate.x;
	result.m[2][1] = translate.y;
	result.m[2][2] = 1;

	return result;
}

Math::Matrix3x3 Math::Vector2::MakeAffine(const Math::Vector2& scale, float theta, const Math::Vector2& translate) {
	Math::Matrix3x3 scaleMatrix{};
	Math::Matrix3x3 rotateMatrix{};
	Math::Matrix3x3 translateMatrix{};
	Math::Matrix3x3 worldMatrix{};

	scaleMatrix = MakeScaleMat(scale);
	rotateMatrix = MakeRotateMat(theta);
	translateMatrix = MakeTranslateMat(translate);

	worldMatrix = Math::Matrix3x3::Multiply(Math::Matrix3x3::Multiply(scaleMatrix, rotateMatrix), translateMatrix);

	return worldMatrix;
}