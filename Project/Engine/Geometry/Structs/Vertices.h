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

struct TriangleVertices {
	Vector4 vertex1;// 左下
	Vector4 vertex2;// 上
	Vector4 vertex3;// 右下
};

struct RectangleVertices {
	Vector4 leftTop;
	Vector4 rightTop;
	Vector4 leftBottom;
	Vector4 rightBottom;
};


