#include "PlaneGeometry.h"

void PlaneGeometry::Init(const Vector2& sizeRaito) {
	geometryName_ = "planeGeometry";

	// 頂点数を決定しておく
	vertexData_.resize(4);
	indices_.resize(6);

	RectangleVertices rect{};
	rect.SetSize(sizeRaito);

	// 座標の設定
	vertexData_[0].pos = rect.leftBottom;
	vertexData_[1].pos = rect.leftTop;
	vertexData_[2].pos = rect.rightBottom;
	vertexData_[3].pos = rect.rightTop;

	// texcoordの設定
	vertexData_[0].texcoord = { 0.0f, 1.0f };
	vertexData_[1].texcoord = { 0.0f, 0.0f };
	vertexData_[2].texcoord = { 1.0f, 1.0f };
	vertexData_[3].texcoord = { 1.0f, 0.0f };

	// 法線の設定
	vertexData_[0].normal = CVector3::FORWARD;
	vertexData_[1].normal = CVector3::FORWARD;
	vertexData_[2].normal = CVector3::FORWARD;
	vertexData_[3].normal = CVector3::FORWARD;

	// indexの設定
	indices_[0] = 0;
	indices_[1] = 1;
	indices_[2] = 2;

	indices_[3] = 1;
	indices_[4] = 3;
	indices_[5] = 2;

}
