#include "PlaneGeometry.h"

void PlaneGeometry::Init() {
	// 頂点数を決定しておく
	vertexData_.resize(4);

	vertexData_[0].pos = { -1.0f, -1.0f, 0.0f, 1.0f };
	vertexData_[1].pos = { -1.0f, -1.0f, 0.0f, 1.0f };
	vertexData_[2].pos = { -1.0f, -1.0f, 0.0f, 1.0f };
	vertexData_[3].pos = { -1.0f, -1.0f, 0.0f, 1.0f };

}
