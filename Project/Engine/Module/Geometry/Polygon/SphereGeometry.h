#pragma once
#include <string>
#include "Engine/Module/Geometry/Polygon/IGeometry.h"

class SphereGeometry final
	: public IGeometry {
public:

	SphereGeometry() = default;
	~SphereGeometry() override = default;

	void Init(const Vector2& size = CVector2::UNIT, uint32_t division = 16, const std::string& name = "sphereGeometry");

};

