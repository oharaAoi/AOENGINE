#include "Color.h"

using namespace AOENGINE;

Color Color::Lerp(const Color& _a, const Color& _b, float _t) {
	return Color(
		std::lerp(_a.r, _b.r, _t),
		std::lerp(_a.g, _b.g, _t),
		std::lerp(_a.b, _b.b, _t),
		std::lerp(_a.a, _b.a, _t)
	);
}

Color Color::RgbTo01(float _r, float _g, float _b) {
	return Color(_r / 255.0f, _g / 255.0f, _b / 255.0f, 1.0f);
}
