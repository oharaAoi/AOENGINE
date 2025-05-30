#pragma once
#include "Engine/Module/Geometry/Polygon/IGeometry.h"

/// <summary>
/// 筒形状
/// </summary>
class CylinderGeometry final 
	: public IGeometry {
public:

	CylinderGeometry() = default;
	~CylinderGeometry() override = default;

	void Init(uint32_t division = 32, float bottomRadius = 1.0f, float topRadius = 1.0f, float height = 2.0f);
	void Init(uint32_t division = 32, Vector2 bottomRadius = CVector2::UNIT, Vector2 topRadius = CVector2::UNIT, float height = 2.0f);
};

