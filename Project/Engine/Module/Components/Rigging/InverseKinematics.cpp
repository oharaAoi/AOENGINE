#include "InverseKinematics.h"

void FABRIK(std::vector<Vector3>& born, const Vector3& target, float tolerance, uint32_t maxIter) {
	uint32_t bornNum = uint32_t(born.size() - 1);
	std::vector<float> distances(bornNum);
	float totalLength = 0;

	// 各ボーンの長さを計算
	for (uint32_t i = 0; i < bornNum; i++) {
		distances[i] = Distance(born[i], born[i + 1]);
		totalLength += distances[i];
	}

	// ターゲットが届かない時
	if (Distance(born[0], target) > totalLength) {
		for (uint32_t i = 0; i < bornNum; ++i) {
			float r = Distance(target, born[i]);
			float lambda = distances[i] / r;
			born[i + 1] = (1 - lambda) * born[i] + lambda * target;
		}
		return;
	}

	// FABRIK反復
	Vector3 base = born[0];
	for (uint32_t iter = 0; iter < maxIter; ++iter) {
		// 前進ステップ
		born[bornNum] = target;
		for (uint32_t i = bornNum - 1; i > 0; --i) {
			float r = Distance(born[i + 1], born[i]);
			float lambda = distances[i] / r;
			born[i] = (1 - lambda) * born[i + 1] + lambda * born[i];
		}

		// 後退ステップ
		born[0] = base;
		for (uint32_t i = 0; i < bornNum; ++i) {
			float r = Distance(born[i + 1], born[i]);
			float lambda = distances[i] / r;
			born[i + 1] = (1 - lambda) * born[i] + lambda * born[i + 1];
		}

		// 収束判定
		if (Distance(born[bornNum], target) < tolerance) { break; }
	}
}
