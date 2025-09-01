#pragma once
#include <vector>
#include <Engine/Lib/Math/Vector3.h>
#include <Engine/Lib/Math/MyMath.h>

void FABRIK(std::vector<Vector3>& born, const Vector3& target, float tolerance = 0.001f, uint32_t maxIter = 10);
