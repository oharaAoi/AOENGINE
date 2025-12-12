#include "BossEvaluationFormula.h"
#include <cmath>
#include <algorithm>

float BossEvaluationFormula::AppropriateDistance(float _distance, float _optimal, float _range) {
	float distance = std::abs(_distance - _optimal) / _range;
	return 1.0f - std::clamp(distance, 0.0f, 1.0f);
}
