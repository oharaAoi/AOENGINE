#include "DrawUtils.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Grid線の描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void DrawGrid(const Math::Matrix4x4& viewMatrix, const Math::Matrix4x4& projectionMatrix) {
	//const float kGridHalfwidth_ = 20.0f; // 中心からの半幅
	const uint32_t kSubdivision_ = 60;   // 分割数
	const float gridLength = 30.0f;      // グリッドの全体幅
	const float kGridEvery_ = 1.0f;      // グリッドの1マスのサイズ

	// X軸に平行なグリッド線の描画
	for (uint32_t xIndex = 0; xIndex <= kSubdivision_; ++xIndex) {
		float half = float(kSubdivision_) / 2.0f;

		// 始点と終点
		Math::Vector3 stPos = { -gridLength, 0, (xIndex - half) * kGridEvery_ };
		Math::Vector3 endPos = { gridLength, 0, (xIndex - half) * kGridEvery_ };

		// 中央軸ラインの色を変更
		if (xIndex == kSubdivision_ / 2) {
			AOENGINE::Render::DrawLine(stPos, endPos, AOENGINE::Color::blue, Multiply(viewMatrix, projectionMatrix));
		} else {
			// 他のグリッド線
			AOENGINE::Render::DrawLine(stPos, endPos, AOENGINE::Color(0.8f, 0.8f, 0.8f, 1.0f), Multiply(viewMatrix, projectionMatrix));
		}
	}

	// Z軸に平行なグリッド線の描画
	for (uint32_t zIndex = 0; zIndex <= kSubdivision_; ++zIndex) {
		float half = float(kSubdivision_) / 2.0f;

		// 始点と終点
		Math::Vector3 stPos = { (zIndex - half) * kGridEvery_, 0, gridLength };
		Math::Vector3 endPos = { (zIndex - half) * kGridEvery_, 0, -gridLength };

		// 中央軸ラインの色を変更
		if (zIndex == kSubdivision_ / 2) {
			AOENGINE::Render::DrawLine(stPos, endPos, AOENGINE::Color::red, Multiply(viewMatrix, projectionMatrix));
		} else {
			// 他のグリッド線
			AOENGINE::Render::DrawLine(stPos, endPos, AOENGINE::Color(0.8f, 0.8f, 0.8f, 1.0f), Multiply(viewMatrix, projectionMatrix));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　球の描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void DrawSphere(const Math::Vector3& center, float radius, const Math::Matrix4x4& viewProjectionMatrix, const AOENGINE::Color& color) {
	const uint32_t kSubdivision = 8;
	const float kLonEvery = 2.0f * float(M_PI) / kSubdivision;
	const float kLatEvery = float(M_PI) / kSubdivision;

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex; // 現在の緯度 theta

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery; // 現在の経度 fai

			Math::Vector3 a, b, c;
			Math::Vector3 localA{}, localB{}, localC{};
			localA = {
				std::cos(lat) * cos(lon) * (radius),
				std::sin(lat) * (radius),
				std::cos(lat) * std::sin(lon) * (radius)
			};

			localB = {
				std::cos(lat + kLatEvery) * std::cos(lon) * (radius) ,
				std::sin(lat + kLatEvery) * (radius),
				std::cos(lat + kLatEvery) * std::sin(lon) * (radius)
			};

			localC = {
				std::cos(lat) * std::cos(lon + kLonEvery) * (radius),
				std::sin(lat) * (radius),
				std::cos(lat) * std::sin(lon + kLonEvery) * (radius)
			};


			a = {
				localA.x + center.x,
				localA.y + center.y,
				localA.z + center.z,
			};

			b = {
				localB.x + center.x,
				localB.y + center.y,
				localB.z + center.z,
			};

			c = {
				localC.x + center.x,
				localC.y + center.y,
				localC.z + center.z,
			};

			AOENGINE::Render::DrawLine(a, b, color, viewProjectionMatrix);
			AOENGINE::Render::DrawLine(a, c, color, viewProjectionMatrix);
		}
	}
}

void DrawCone(const Math::Vector3& center, const Math::Quaternion& rotate, float radius, float angle, float height, const Math::Matrix4x4& viewProjectionMatrix) {
	const uint32_t segment = 36;
	float angleIncrement = 2.0f * kPI / segment;
	const Math::Vector3 rotateHeight = rotate * Math::Vector3(0, height, 0);
	const Math::Vector3 coneTip = center + rotateHeight; // 円錐の頂点（Y軸方向を高さとして仮定）
	float newRadius = height * std::tanf(angle * 0.5f);

	std::vector<Math::Vector3> basePoints(segment);
	for (int i = 0; i < segment; ++i) {
		float theta = i * angleIncrement;
		basePoints[i] = center + radius * Math::Vector3(cos(theta), 0, sin(theta));
		basePoints[i] = (rotate * (basePoints[i] - center)) + center;  // 回転適用
	}

	std::vector<Math::Vector3> topPoints(segment);
	for (int i = 0; i < segment; ++i) {
		float theta = i * angleIncrement;
		topPoints[i] = center + newRadius * Math::Vector3(cos(theta), 0, sin(theta));
		topPoints[i] = (rotate * (topPoints[i] - center)) + center;  // 回転適用
	}

	// 描画
	for (int i = 0; i < segment; ++i) {
		// 現在の点と次の点を結ぶ（円の線）
		Math::Vector3 p1 = basePoints[i];
		Math::Vector3 p2 = basePoints[(i + 1) % segment];

		Math::Vector3 topP1 = topPoints[i] + rotateHeight;
		Math::Vector3 topP2 = topPoints[(i + 1) % segment] + rotateHeight;

		AOENGINE::Render::DrawLine(p1, p2, AOENGINE::Color::green, viewProjectionMatrix);
		AOENGINE::Render::DrawLine(topP1, topP2, AOENGINE::Color::green, viewProjectionMatrix);

		if (i % 9 == 0) {
			// 頂点と底面の点を結ぶ（側面の線）
			AOENGINE::Render::DrawLine(topP1, p1, AOENGINE::Color::green, viewProjectionMatrix);
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　AABBの描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void DrawAABB(const Math::AABB& aabb, const Math::Matrix4x4& vpMatrix, const AOENGINE::Color& color) {
	std::array<Math::Vector3, 8> point = {
		Math::Vector3{aabb.min.x,aabb.max.y, aabb.min.z }, // front_LT
		Math::Vector3{aabb.max.x,aabb.max.y, aabb.min.z }, // front_RT
		Math::Vector3{aabb.max.x,aabb.min.y, aabb.min.z }, // front_RB
		Math::Vector3{aabb.min.x,aabb.min.y, aabb.min.z }, // front_LB
		Math::Vector3{aabb.min.x,aabb.max.y, aabb.max.z }, // back_LT
		Math::Vector3{aabb.max.x,aabb.max.y, aabb.max.z }, // back_RT
		Math::Vector3{aabb.max.x,aabb.min.y, aabb.max.z }, // back_RB
		Math::Vector3{aabb.min.x,aabb.min.y, aabb.max.z }, // back_LB
	};

	for (uint32_t oi = 0; oi < 4; oi++) {
		AOENGINE::Render::DrawLine(point[oi], point[(oi + 1) % 4], color, vpMatrix);
		uint32_t j = oi + 4;
		AOENGINE::Render::DrawLine(point[j], point[(j + 1) % 4 + 4], color, vpMatrix);
		AOENGINE::Render::DrawLine(point[oi], point[j], color, vpMatrix);
	}


}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　OBBの描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void DrawOBB(const Math::OBB& obb, const Math::Matrix4x4& vpMatrix, const AOENGINE::Color& color) {
	Math::Matrix4x4 rotateMatrix = obb.matRotate;
	// 平行移動分を作成
	Math::Matrix4x4 matTranslate = obb.center.MakeTranslateMat();
	// 拡縮分
	Math::Matrix4x4 matScale = Math::Vector3({ 1.0f,1.0f, 1.0f }).MakeScaleMat();
	// ワールド行列を求める
	Math::Matrix4x4 worldMat = matScale * rotateMatrix * matTranslate;

	// ローカルの頂点を求める
	Math::Vector3 min = obb.size * -1;
	Math::Vector3 max = obb.size;
	std::array<Math::Vector3, 8> point = {
		Math::Vector3{min.x, max.y, min.z } *worldMat, // front_LT
		Math::Vector3{max.x, max.y, min.z } *worldMat, // front_RT
		Math::Vector3{max.x, min.y, min.z } *worldMat, // front_RB
		Math::Vector3{min.x, min.y, min.z } *worldMat, // front_LB
		Math::Vector3{min.x, max.y, max.z } *worldMat, // back_LT
		Math::Vector3{max.x, max.y, max.z } *worldMat, // back_RT
		Math::Vector3{max.x, min.y, max.z } *worldMat, // back_RB
		Math::Vector3{min.x, min.y, max.z } *worldMat, // back_LB
	};


	for (uint32_t oi = 0; oi < 4; oi++) {
		AOENGINE::Render::DrawLine(point[oi], point[(oi + 1) % 4], color, vpMatrix);
		uint32_t j = oi + 4;
		AOENGINE::Render::DrawLine(point[j], point[(j + 1) % 4 + 4], color, vpMatrix);
		AOENGINE::Render::DrawLine(point[oi], point[j], color, vpMatrix);
	}
}
