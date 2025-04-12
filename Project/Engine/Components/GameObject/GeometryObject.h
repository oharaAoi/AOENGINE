#pragma once
#include <memory>
#include "Engine/Geometry/GeometryManager.h"
#include "Engine/Components/Meshes/Mesh.h"
#include "Engine/Components/Materials/Material.h"
#include "Engine/Components/WorldTransform.h"

/// <summary>
/// Geometryの各形状をScene上に描画する際に使用するクラス
/// </summary>
class GeometryObject final {
public:

	GeometryObject() = default;
	~GeometryObject() = default;

	// 平面をSet
	void SetPlane(const Vector2& size = CVector2::UNIT);
	void SetSphere(const Vector2& size = CVector2::UNIT, uint32_t division = 16);
	void SetCube(const Vector3& size = CVector3::UNIT);
	void SetRing(uint32_t division = 32, float outerRadius = 1.0f, float innerRadius = 0.2f);
	void SetCylinder(uint32_t division = 32, float radius = 1.0f, float height = 2.0f);

	void Update();
	void Draw() const;

private:

	/// <summary>
	/// 初期化は共通して同じ
	/// </summary>
	void Init();

private:

	GeometryType type_;

	std::unique_ptr<Mesh> mesh_;
	std::unique_ptr<Material> material_ = nullptr;
	std::unique_ptr<WorldTransform> transform_ = nullptr;

};

