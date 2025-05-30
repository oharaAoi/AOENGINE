#include "CylinderGeometry.h"
#include "Engine/Lib/Math/MyMath.h"

void CylinderGeometry::Init(uint32_t division, float bottomRadius, float topRadius, float height) {
	geometryName_ = "cylinderGeometry";

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

		Vector3 outerNormal = Vector3(-sin, 0.0f, cos).Normalize();
		Vector3 outerNormalNext = Vector3(-sinNext, 0.0f, cosNext).Normalize();

		uint32_t oi = index * 4;
		// 実際に入力
		vertexData_[oi].pos = { -sin * bottomRadius, height, cos * bottomRadius, 1.0f };
		vertexData_[oi].texcoord = { u, 0.0f };
		vertexData_[oi].normal = outerNormal;

		vertexData_[oi + 1].pos = { -sinNext * bottomRadius, height, cosNext * bottomRadius, 1.0f };
		vertexData_[oi + 1].texcoord = { uNext, 0.0f };
		vertexData_[oi + 1].normal = outerNormalNext;

		vertexData_[oi + 2].pos = { -sin * topRadius, 0.0f, cos * topRadius, 1.0f };
		vertexData_[oi + 2].texcoord = { u, 1.0f };
		vertexData_[oi + 2].normal = outerNormal;

		vertexData_[oi + 3].pos = { -sinNext * topRadius, 0.0f,cosNext * topRadius, 1.0f };
		vertexData_[oi + 3].texcoord = { uNext, 1.0f };
		vertexData_[oi + 3].normal = outerNormalNext;

		uint32_t ii = index * 6;
		indices_[ii + 0] = oi + 0;
		indices_[ii + 1] = oi + 1;
		indices_[ii + 2] = oi + 2;

		indices_[ii + 3] = oi + 1;
		indices_[ii + 4] = oi + 3;
		indices_[ii + 5] = oi + 2;
	}
}

void CylinderGeometry::Init(uint32_t division, Vector2 bottomRadius, Vector2 topRadius, float height) {
	geometryName_ = "cylinderGeometryCollapsed";

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

		Vector3 outerNormal = Vector3(-sin, 0.0f, cos).Normalize();
		Vector3 outerNormalNext = Vector3(-sinNext, 0.0f, cosNext).Normalize();

		uint32_t oi = index * 4;
		// 実際に入力
		vertexData_[oi].pos = { -sin * bottomRadius.x, height, cos * bottomRadius.y, 1.0f };
		vertexData_[oi].texcoord = { u, 0.0f };
		vertexData_[oi].normal = outerNormal;

		vertexData_[oi + 1].pos = { -sinNext * bottomRadius.x, height, cosNext * bottomRadius.y, 1.0f };
		vertexData_[oi + 1].texcoord = { uNext, 0.0f };
		vertexData_[oi + 1].normal = outerNormalNext;

		vertexData_[oi + 2].pos = { -sin * topRadius.x, 0.0f, cos * topRadius.y, 1.0f };
		vertexData_[oi + 2].texcoord = { u, 1.0f };
		vertexData_[oi + 2].normal = outerNormal;

		vertexData_[oi + 3].pos = { -sinNext * topRadius.x, 0.0f,cosNext * topRadius.y, 1.0f };
		vertexData_[oi + 3].texcoord = { uNext, 1.0f };
		vertexData_[oi + 3].normal = outerNormalNext;

		uint32_t ii = index * 6;
		indices_[ii + 0] = oi + 0;
		indices_[ii + 1] = oi + 1;
		indices_[ii + 2] = oi + 2;

		indices_[ii + 3] = oi + 1;
		indices_[ii + 4] = oi + 3;
		indices_[ii + 5] = oi + 2;
	}
}
