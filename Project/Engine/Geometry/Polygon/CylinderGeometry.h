#pragma once
#include "Engine/Geometry/Polygon/IGeometry.h"

/// <summary>
/// 筒形状
/// </summary>
class CylinderGeometry final 
	: public IGeometry {
public:

	CylinderGeometry() = default;
	~CylinderGeometry() override = default;

	void Init(uint32_t division = 32, float radius = 1.0f, float height = 2.0f);
};

