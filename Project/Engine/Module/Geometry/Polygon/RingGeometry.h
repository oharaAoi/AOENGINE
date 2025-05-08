#pragma once
#include "Engine/Module/Geometry/Polygon/IGeometry.h"

/// <summary>
/// Ring形状
/// </summary>
class RingGeometry final 
	: public IGeometry {
public:

	RingGeometry() = default;
	~RingGeometry() override = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="division">: 分割数</param>
	/// <param name="outerRadius">: 外径</param>
	/// <param name="innerRadius">: 内径</param>
	void Init(uint32_t division = 32, float outerRadius = 1.0f, float innerRadius = 0.2f);

};

