#pragma once
#include <vector>
#include <Engine/Lib/Math/Vector3.h>
#include <Engine/Lib/Math/MyMath.h>

/// <summary>
/// IKの処理
/// </summary>
/// <param name="born"></param>
/// <param name="target"></param>
/// <param name="tolerance"></param>
/// <param name="maxIter"></param>
void FABRIK(std::vector<Math::Vector3>& born, const Math::Vector3& target, float tolerance = 0.001f, uint32_t maxIter = 10);
