#include "RingGeometry.h"
#include "Engine/Lib/Math/MyMath.h"

void RingGeometry::Init(uint32_t division, float outerRadius, float innerRadius) {
	geometryName_ = "ringGeometry";

	const float radianPreDivde = kPI2 / float(division);

	// 頂点数を決定しておく
	vertexData_.resize(division * 4);
	indices_.resize(division * 6);

	for (uint32_t index = 0; index < division; ++index) {
		float sin = std::sinf(index * radianPreDivde);
		float cos = std::cosf(index * radianPreDivde);
		float sinNext = std::sinf((index + 1) * radianPreDivde);
		float cosNext = std::cosf((index + 1) * radianPreDivde);
		float u = float(index) / float(division);
		float uNext = float(index + 1) / float(division);

		uint32_t oi = index * 4;
		// 実際に入力
		vertexData_[oi].pos = { -sin * outerRadius, cos * outerRadius, 0.0f, 1.0f };
		vertexData_[oi].texcoord = { u, 0.0f };
		vertexData_[oi].normal = CVector3::FORWARD;

		vertexData_[oi + 1].pos = { -sinNext * outerRadius, cosNext * outerRadius, 0.0f, 1.0f };
		vertexData_[oi + 1].texcoord = { uNext, 0.0f };
		vertexData_[oi + 1].normal = CVector3::FORWARD;

		vertexData_[oi + 2].pos = { -sin * innerRadius, cos * innerRadius, 0.0f, 1.0f };
		vertexData_[oi + 2].texcoord = { u, 1.0f };
		vertexData_[oi + 2].normal = CVector3::FORWARD;

		vertexData_[oi + 3].pos = { -sinNext * innerRadius, cosNext * innerRadius, 0.0f, 1.0f };
		vertexData_[oi + 3].texcoord = { uNext, 1.0f };
		vertexData_[oi + 3].normal = CVector3::FORWARD;

		uint32_t ii = index * 6;
		indices_[ii + 0] = oi + 0;
		indices_[ii + 1] = oi + 1;
		indices_[ii + 2] = oi + 2;

		indices_[ii + 3] = oi + 1;
		indices_[ii + 4] = oi + 3;
		indices_[ii + 5] = oi + 2;
	}
}
