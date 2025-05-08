#pragma once
#include "Engine/Module/Geometry/Polygon/IGeometry.h"

/// <summary>
/// 平面の形状
/// </summary>
class PlaneGeometry final 
	: public IGeometry {
public:

	PlaneGeometry() = default;
	~PlaneGeometry() override = default;

	void Init(const Vector2& sizeRaito = CVector2::UNIT);

};

