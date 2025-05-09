#include "CubeGeometry.h"
#include "Engine/Lib/Math/Vector3.h"

void CubeGeometry::Init(const Vector3& size) {
	geometryName_ = "cubeGeometry";

	// 頂点数を決定しておく
	vertexData_.resize(24);
	indices_.resize(36);

	Vector3 half = size;

	Vector3 normalTable[6] = {
		CVector3::FORWARD * -1,  // front（-Z）
		CVector3::FORWARD,       // back（+Z）
		CVector3::RIGHT * -1,    // left（-X）
		CVector3::RIGHT,         // right（+X）
		CVector3::UP,            // top（+Y）
		CVector3::UP * -1        // bottom（-Y）
	};

	// 各面の頂点（左下, 左上, 右下, 右上）の順
	Vector3 faceVertices[6][4] = {
		// Front (-Z)
		{ {-half.x, -half.y, -half.z}, {-half.x,  half.y, -half.z}, { half.x, -half.y, -half.z}, { half.x,  half.y, -half.z} },

		// Back (+Z)
		{ { half.x, -half.y, half.z},  { half.x,  half.y, half.z},  {-half.x, -half.y, half.z},  {-half.x,  half.y, half.z} },

		// Left (-X)
		{ {-half.x, -half.y,  half.z}, {-half.x,  half.y,  half.z}, {-half.x, -half.y, -half.z}, {-half.x,  half.y, -half.z} },

		// Right (+X)
		{ { half.x, -half.y, -half.z}, { half.x,  half.y, -half.z}, { half.x, -half.y,  half.z}, { half.x,  half.y,  half.z} },

		// Top (+Y)
		{ {-half.x,  half.y, -half.z}, {-half.x,  half.y,  half.z}, { half.x,  half.y, -half.z}, { half.x,  half.y,  half.z} },

		// Bottom (-Y)
		{ {-half.x, -half.y,  half.z}, {-half.x, -half.y, -half.z}, { half.x, -half.y,  half.z}, { half.x, -half.y, -half.z} },
	};

	// テクスチャ座標（共通）
	Vector2 texcoords[4] = {
		{ 0.0f, 1.0f },  // 左下
		{ 0.0f, 0.0f },  // 左上
		{ 1.0f, 1.0f },  // 右下
		{ 1.0f, 0.0f }   // 右上
	};

	for (int face = 0; face < 6; ++face) {
		Vector3 normal = normalTable[face];
		for (int i = 0; i < 4; ++i) {
			int idx = face * 4 + i;
			vertexData_[idx].pos = faceVertices[face][i];
			vertexData_[idx].normal = normal;
			vertexData_[idx].texcoord = texcoords[i];
		}

		// 頂点インデックスを面の向きに合わせて組む（左下→左上→右下→右上）
		uint32_t vi = face * 4;
		indices_[face * 6 + 0] = vi + 0;
		indices_[face * 6 + 1] = vi + 1;
		indices_[face * 6 + 2] = vi + 2;

		indices_[face * 6 + 3] = vi + 1;
		indices_[face * 6 + 4] = vi + 3;
		indices_[face * 6 + 5] = vi + 2;
	}
}

void CubeGeometry::Inverse() {
	for (int face = 0; face < 6; ++face) {
		// 頂点インデックスを面の向きに合わせて組む（左下→左上→右下→右上）
		uint32_t vi = face * 4;
		indices_[face * 6 + 0] = vi + 2;
		indices_[face * 6 + 1] = vi + 1;
		indices_[face * 6 + 2] = vi + 0;

		indices_[face * 6 + 3] = vi + 2;
		indices_[face * 6 + 4] = vi + 3;
		indices_[face * 6 + 5] = vi + 1;
	}
}
