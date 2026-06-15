#include "Matrix4x4.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Lib/Math/Matrix3x3.h"
#include "Engine/Lib/Math/MyMath.h"

using namespace Math;

Math::Matrix4x4 Math::Matrix4x4::Transpose() const {
	Math::Matrix4x4 result{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			// 行と列を入れ替える
			result.m[i][j] = m[j][i];
		}
	}
	return result;
}

Math::Vector3 Math::Matrix4x4::GetScale() const {
	Math::Vector3 scale;
	scale.x = sqrtf(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);
	scale.y = sqrtf(m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1]);
	scale.z = sqrtf(m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2]);
	return scale;
}

Math::Vector3 Math::Matrix4x4::GetPosition() const {
	return Math::Vector3{ m[3][0], m[3][1], m[3][2] };
}

Math::Quaternion Math::Matrix4x4::GetRotate() const {
	Math::Vector3 xAxis = Math::Vector3(m[0][0], m[1][0], m[2][0]); // 1列目
	Math::Vector3 yAxis = Math::Vector3(m[0][1], m[1][1], m[2][1]); // 2列目
	Math::Vector3 zAxis = Math::Vector3(m[0][2], m[1][2], m[2][2]); // 3列目

	xAxis = Normalize(xAxis);
	yAxis = Normalize(yAxis);
	zAxis = Normalize(zAxis);

	Math::Matrix3x3 mat;
	mat.SetRow(0, xAxis);
	mat.SetRow(1, yAxis);
	mat.SetRow(2, zAxis);

	return Math::Quaternion::RotateMatrixTo(mat);
}

Math::Matrix4x4 Math::Matrix4x4::MakeUnit() {
	Math::Matrix4x4 result{};
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			if (row == col) {
				result.m[row][col] = 1.0f;
			} else {
				result.m[row][col] = 0.0f;
			}
		}
	}
	return result;
}

Math::Matrix4x4 Math::Matrix4x4::MakeAffine(const Math::Matrix4x4& scale, const Math::Matrix4x4& rotate, const Math::Matrix4x4& translate) {
	return Multiply(Multiply(scale, rotate), translate);
}

Math::Matrix4x4 Math::Matrix4x4::MakeAffine(const Math::Vector3& scale, const Math::Vector3& rotate, const Math::Vector3& translate) {
	Math::Matrix4x4 scaleMat = scale.MakeScaleMat();
	Math::Matrix4x4 rotateMat = rotate.MakeRotateMat();
	Math::Matrix4x4 translateMat = translate.MakeTranslateMat();
	return MakeAffine(scaleMat, rotateMat, translateMat);
}

Math::Matrix4x4 Math::Matrix4x4::MakeAffine(const Math::Vector3& scale, const Math::Quaternion& rotate, const Math::Vector3& translate) {
	Math::Matrix4x4 scaleMat = scale.MakeScaleMat();
	Math::Matrix4x4 rotateMat = rotate.MakeMatrix();
	Math::Matrix4x4 translateMat = translate.MakeTranslateMat();
	return MakeAffine(scaleMat, rotateMat, translateMat);
}

Math::Matrix4x4 Math::Matrix4x4::MakeOrthograhic(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Math::Matrix4x4 result{};
	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = 1.0f / (farClip - nearClip);
	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[3][3] = 1;
	return result;
}

Math::Matrix4x4 Math::Matrix4x4::MakePerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip) {
	Math::Matrix4x4 result{};
	float tangent = fovY / 2.0f;
	float cotangent = 1.0f / atanf(tangent);
	result.m[0][0] = (1.0f / aspectRatio) * cotangent;
	result.m[1][1] = cotangent;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	return result;
}

Math::Matrix4x4 Math::Matrix4x4::MakeViewport(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Math::Matrix4x4 result{};
	result.m[0][0] = width / 2.0f;
	result.m[1][1] = -height / 2.0f;
	result.m[2][2] = maxDepth - minDepth;
	result.m[3][0] = left + (width / 2.0f);
	result.m[3][1] = top + (height / 2.0f);
	result.m[3][2] = minDepth;
	result.m[3][3] = 1.0f;
	return result;
}

Math::Matrix4x4 Math::Matrix4x4::LookAtLH(const Math::Vector3& _eye, const Math::Vector3& _target, const Math::Vector3& _up) {
	Math::Vector3 z = Normalize(_target - _eye);      // forward
	Math::Vector3 x = Normalize(Cross(_up, z));      // right
	Math::Vector3 y = Cross(z, x);                  // up

	Math::Matrix4x4 m{};
	m.m[0][0] = x.x; m.m[0][1] = y.x; m.m[0][2] = z.x; m.m[0][3] = 0.0f;
	m.m[1][0] = x.y; m.m[1][1] = y.y; m.m[1][2] = z.y; m.m[1][3] = 0.0f;
	m.m[2][0] = x.z; m.m[2][1] = y.z; m.m[2][2] = z.z; m.m[2][3] = 0.0f;

	m.m[3][0] = -Dot(x, _eye);
	m.m[3][1] = -Dot(y, _eye);
	m.m[3][2] = -Dot(z, _eye);
	m.m[3][3] = 1.0f;

	return m;
}

void Math::Matrix4x4::MakeArray(float* mat) {
	// ImGuizmo用に転置してコピー
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			mat[col * 4 + row] = m[row][col]; // 転置
		}
	}
}
