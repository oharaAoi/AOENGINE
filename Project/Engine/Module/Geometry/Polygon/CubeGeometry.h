#pragma once
#include <utility>
#include "Engine/Module/Geometry/Polygon/IGeometry.h"

/// <summary>
/// 箱形状
/// </summary>
class CubeGeometry final
	: public IGeometry {
public:

	CubeGeometry() = default;
	~CubeGeometry() override = default;

	void Init(const Math::Vector3& size = CVector3::UNIT);

	void Inverse();
};

