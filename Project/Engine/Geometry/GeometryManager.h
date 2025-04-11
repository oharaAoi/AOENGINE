#pragma once
#include "Engine/Geometry/Polygon/PlaneGeometry.h"
#include "Engine/Components/Meshes/Mesh.h"

enum class GeometryType {
	PLANE, // 平面
};

/// <summary>
/// 各Geometryを管理しているクラス
/// </summary>
class GeometryManager final {
public:

	GeometryManager() = default; // コンストラクタをprivateに（シングルトン）
	GeometryManager(const GeometryManager&) = delete;
	GeometryManager& operator=(const GeometryManager&) = delete;

	static GeometryManager& GetInstance();

	void Init();

	void SetPlane(Mesh* _pMesh, const Vector2& size);

};

