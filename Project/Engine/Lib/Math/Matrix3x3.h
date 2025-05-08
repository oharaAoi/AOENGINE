#pragma once
#include "Engine/Lib/Math/Vector3.h"

class Matrix3x3 final {
public:
	float m[3][3];

	//3x3の行列の積
	static Matrix3x3 Multiply(const Matrix3x3& matrix1, const Matrix3x3& matrix2);

	void SetRow(int row, const Vector3& v) {
		m[row][0] = v.x;
		m[row][1] = v.y;
		m[row][2] = v.z;
	}
};