#pragma once
#include "Engine/Module/Geometry/Polygon/IGeometry.h"
#include "Engine/Lib/Math/Vector2.h"

namespace AOENGINE {

/// <summary>
/// 平面の形状
/// </summary>
class PlaneGeometry final
	: public IGeometry {
public:

	PlaneGeometry() = default;
	~PlaneGeometry() override = default;

	void Init(const Math::Vector2& sizeRaito = CMath::Vector2::UNIT);

};

}