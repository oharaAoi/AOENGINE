#include "Vector4.h"

using namespace Math;

Math::Vector4 Math::Vector4::Lerp(const Math::Vector4& start, const Math::Vector4& end, float t) {
    Math::Vector4 result{};
    result.x = std::lerp(start.x, end.x, t);
    result.y = std::lerp(start.y, end.y, t);
    result.z = std::lerp(start.z, end.z, t);
    result.w = std::lerp(start.w, end.w, t);
    return result;
}
