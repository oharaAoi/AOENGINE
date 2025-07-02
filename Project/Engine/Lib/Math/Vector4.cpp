#include "Vector4.h"

Vector4 Vector4::Lerp(const Vector4& start, const Vector4& end, float t) {
    Vector4 result{};
    result.x = std::lerp(start.x, end.x, t);
    result.y = std::lerp(start.y, end.y, t);
    result.z = std::lerp(start.z, end.z, t);
    result.w = std::lerp(start.w, end.w, t);
    return result;
}
