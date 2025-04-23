#include "MyRandom.h"

float RandomFloat(const float& min, const float& max) {
    std::uniform_real_distribution<float> distribution(min, max);
    float result = distribution(randomEngine);
    return result;
}

int RandomInt(int min, int max){
    std::uniform_int_distribution<int> distribution(min, max);
    int result = distribution(randomEngine);
    return result;
}

Vector3 RandomVector3(const Vector3& min, const Vector3& max) {
    Vector3 result;

    result.x = RandomFloat(min.x, max.x);
    result.y = RandomFloat(min.y, max.y);
    result.z = RandomFloat(min.z, max.z);

    return result;
}
