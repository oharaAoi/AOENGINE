#pragma once
#include <vector>
#include "Engine/Geometry/Structs/Vertices.h"

/// <summary>
/// 平面の形状
/// </summary>
class PlaneGeometry final {
public:

	PlaneGeometry() = default;
	~PlaneGeometry() = default;

	void Init();

private:

	std::vector<VertexData> vertexData_;

};

