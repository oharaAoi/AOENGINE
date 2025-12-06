#pragma once
#include <string>
#include "Engine/Module/Geometry/Polygon/IGeometry.h"
#include "Engine/Lib/Math/Vector2.h"

namespace AOENGINE {

/// <summary>
/// 球形状
/// </summary>
class SphereGeometry final
	: public IGeometry {
public:

	SphereGeometry() = default;
	~SphereGeometry() override = default;

	void Init(const Math::Vector2& size = CMath::Vector2::UNIT, uint32_t division = 16, const std::string& name = "sphereGeometry");

	void Debug_Gui();

private:
	Math::Vector2 size_;
	uint32_t division_;
};

}