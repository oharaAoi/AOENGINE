#include "Color.h"

Color Color::Lerp(const Color& _a, const Color& _b, float _t) {
	return Color(
		std::lerp(_a.r, _b.r, _t),
		std::lerp(_a.g, _b.g, _t),
		std::lerp(_a.b, _b.b, _t),
		std::lerp(_a.a, _b.a, _t)
	);
}

Color Color::RgbTo01(float r, float g, float b) {
	return Color(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
}
