#pragma once
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
	void SetSphere(Mesh* _pMesh, const Vector2& size, uint32_t division);
	void SetCube(Mesh* _pMesh, const Vector3& size);
	void SetRing(Mesh* _pMesh, uint32_t division, float outerRadius, float innerRadius);
	void SetCylinder(Mesh* _pMesh, uint32_t division, float radius, float height);

};

