#pragma once
#include <vector>
#include "Engine/Geometry/Structs/Vertices.h"

class IGeometry {
public:

	IGeometry() = default;
	virtual ~IGeometry() = default;

	const std::vector<VertexData>& GetVertex() const { return vertexData_; }

	const std::vector<uint32_t>& GetIndex() const { return indices_; }

protected:

	std::vector<VertexData> vertexData_;
	std::vector<uint32_t> indices_;

};

