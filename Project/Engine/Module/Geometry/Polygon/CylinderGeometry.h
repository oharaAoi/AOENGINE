#pragma once
#include "Engine/Module/Geometry/Polygon/IGeometry.h"
#include "Engine/Lib/Math/Vector2.h"

namespace AOENGINE {

/// <summary>
/// 筒形状
/// </summary>
class CylinderGeometry final
	: public AOENGINE::IGeometry {
public:

	CylinderGeometry() = default;
	~CylinderGeometry() override = default;

	void Init(uint32_t division = 32, float bottomRadius = 1.0f, float topRadius = 1.0f, float height = 2.0f);
	void Init(uint32_t division = 32, Math::Vector2 bottomRadius = CMath::Vector2::UNIT, Math::Vector2 topRadius = CMath::Vector2::UNIT, float height = 2.0f);
};

}