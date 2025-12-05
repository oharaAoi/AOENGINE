#include "Matrix3x3.h"

using namespace Math;

Math::Matrix3x3 Math::Matrix3x3::Multiply(const Math::Matrix3x3& matrix1, const Math::Matrix3x3& matrix2) {
	Math::Matrix3x3 result{};
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			for (int i = 0; i < 3; i++) {
				result.m[row][col] += matrix1.m[row][i] * matrix2.m[i][col];
			}
		}
	}

	return result;
}
