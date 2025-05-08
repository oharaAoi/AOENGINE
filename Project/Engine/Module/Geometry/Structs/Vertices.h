#pragma once
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"

struct VertexData {
	Vector4 pos;
	Vector2 texcoord;
	Vector3 normal;
	Vector4 worldPos;
	Vector3 tangent;
};

struct ParticleVertexData {
	Vector4 pos;
	Vector2 texcoord;
};

struct TriangleVertices {
	Vector4 vertex1;// 左下
	Vector4 vertex2;// 上
	Vector4 vertex3;// 右下
};

struct RectangleVertices {
	Vector4 leftTop		= {-1.0f, -1.0f, 0.0f, 1.0f};
	Vector4 rightTop	= { 1.0f, -1.0f, 0.0f, 1.0f };
	Vector4 leftBottom	= { -1.0f, 1.0f, 0.0f, 1.0f };
	Vector4 rightBottom = { 1.0f, 1.0f, 0.0f, 1.0f };

	void SetSize(const Vector2& sizeRaito){
		leftTop.x *= sizeRaito.x;
		leftTop.y *= sizeRaito.y;

		rightTop.x *= sizeRaito.x;
		rightTop.y *= sizeRaito.y;

		leftBottom.x *= sizeRaito.x;
		leftBottom.y *= sizeRaito.y;

		rightBottom.x *= sizeRaito.x;
		rightBottom.y *= sizeRaito.y;
	}
};


