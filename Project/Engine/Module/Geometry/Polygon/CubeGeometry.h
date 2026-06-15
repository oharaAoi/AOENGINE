#pragma once
#include <utility>
#include "Engine/Module/Geometry/Polygon/IGeometry.h"
#include "Engine/Lib/Math/Vector3.h"

namespace AOENGINE {

/// <summary>
/// 箱形状
/// </summary>
class CubeGeometry final
	: public AOENGINE::IGeometry {
public:

	CubeGeometry() = default;
	~CubeGeometry() override = default;

	void Init(const Math::Vector3& size = CVector3::UNIT);

	void Inverse();
};

}