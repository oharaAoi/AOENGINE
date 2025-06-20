#include "SphereGeometry.h"
#include "Engine/Lib/Math/MyMath.h"

void SphereGeometry::Init(const Vector2& size, uint32_t division, const std::string& name) {
	geometryName_ = name;

	const float kLonEvery = float(kPI) * 2.0f / float(division);// fai
	const float kLatEvery = float(kPI) / float(division); // theta

	uint32_t start = 0;
	uint32_t startIndex = 0;

	// 頂点数を決定しておく
	vertexData_.resize(division * division * 4);
	indices_.resize(division * division * 6);

	for (uint32_t latIndex = 0; latIndex < division; ++latIndex) {
		float lat = -float(kPI) / 2.0f + kLatEvery * latIndex; // theta
		// 経度方向に分割
		for (uint32_t lonIndex = 0; lonIndex < division; ++lonIndex) {
			start = (latIndex * division + lonIndex) * 4;
			float lon = lonIndex * kLonEvery; // fai
			// 頂点データを入力
			// a 左下
			vertexData_[start].pos.x = std::cos(lat) * std::cos(lon);
			vertexData_[start].pos.y = std::sin(lat);
			vertexData_[start].pos.z = std::cos(lat) * std::sin(lon);
			vertexData_[start].pos.w = 1.0f;

			vertexData_[start].texcoord.x = float(lonIndex) / float(division);
			vertexData_[start].texcoord.y = 1.0f - float(latIndex) / float(division);
			vertexData_[start].normal = { vertexData_[start].pos.x, vertexData_[start].pos.y, vertexData_[start].pos.z };

			// b 左上
			vertexData_[start + 1].pos.x = std::cos(lat + kLatEvery) * std::cos(lon);
			vertexData_[start + 1].pos.y = std::sin(lat + kLatEvery);
			vertexData_[start + 1].pos.z = std::cos(lat + kLatEvery) * std::sin(lon);
			vertexData_[start + 1].pos.w = 1.0f;

			vertexData_[start + 1].texcoord.x = float(lonIndex) / float(division);
			vertexData_[start + 1].texcoord.y = 1.0f - float(latIndex + 1) / float(division);
			vertexData_[start + 1].normal = { vertexData_[start + 1].pos.x, vertexData_[start + 1].pos.y, vertexData_[start + 1].pos.z };

			// c 右下
			vertexData_[start + 2].pos.x = std::cos(lat) * std::cos(lon + kLonEvery);
			vertexData_[start + 2].pos.y = std::sin(lat);
			vertexData_[start + 2].pos.z = std::cos(lat) * std::sin(lon + kLonEvery);
			vertexData_[start + 2].pos.w = 1.0f;

			vertexData_[start + 2].texcoord.x = float(lonIndex + 1) / float(division);
			vertexData_[start + 2].texcoord.y = 1.0f - float(latIndex) / float(division);
			vertexData_[start + 2].normal = { vertexData_[start + 2].pos.x, vertexData_[start + 2].pos.y, vertexData_[start + 2].pos.z };

			// d 右上
			vertexData_[start + 3].pos.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
			vertexData_[start + 3].pos.y = std::sin(lat + kLatEvery);
			vertexData_[start + 3].pos.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
			vertexData_[start + 3].pos.w = 1.0f;

			vertexData_[start + 3].texcoord.x = float(lonIndex + 1) / float(division);
			vertexData_[start + 3].texcoord.y = 1.0f - float(latIndex + 1) / float(division);
			vertexData_[start + 3].normal = { vertexData_[start + 3].pos.x, vertexData_[start + 3].pos.y, vertexData_[start + 3].pos.z };

			//
			startIndex = (latIndex * division + lonIndex) * 6;

			indices_[startIndex] = start;
			indices_[startIndex + 1] = start + 1;
			indices_[startIndex + 2] = start + 2;

			indices_[startIndex + 3] = start + 1;
			indices_[startIndex + 4] = start + 3;
			indices_[startIndex + 5] = start + 2;
		}
	}

	for (uint32_t oi = 0; oi < vertexData_.size(); ++oi) {
		vertexData_[oi].pos.x *= size.x;
		vertexData_[oi].pos.y *= size.y;
		vertexData_[oi].pos.z *= size.x;
	}
}
