#pragma once
#include <random>
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"

inline std::random_device seedGenerator;
inline std::mt19937 randomEngine(seedGenerator());

namespace Random {

float RandomFloat(const float& min, const float& max);
int RandomInt(int min, int max);

Math::Vector3 RandomVector3(const Math::Vector3& min, const Math::Vector3& max);

}