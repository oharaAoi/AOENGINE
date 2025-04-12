#pragma once
#include <utility>
#include "Engine/Geometry/Polygon/IGeometry.h"

class CubeGeometry  final
	: public IGeometry {
public:

	CubeGeometry() = default;
	~CubeGeometry() override = default;

	void Init(const Vector3& size = CVector3::UNIT);
};

