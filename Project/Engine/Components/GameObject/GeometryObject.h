#pragma once
#include <memory>
#include "Engine/Geometry/GeometryFactory.h"
#include "Engine/Components/Meshes/Mesh.h"
#include "Engine/Components/Materials/Material.h"
#include "Engine/Components/WorldTransform.h"
#include "Engine/Geometry/Polygon/PlaneGeometry.h"
#include "Engine/Geometry/Polygon/SphereGeometry.h"
#include "Engine/Geometry/Polygon/CubeGeometry.h"
#include "Engine/Geometry/Polygon/RingGeometry.h"
#include "Engine/Geometry/Polygon/CylinderGeometry.h"

/// <summary>
/// Geometryの各形状をScene上に描画する際に使用するクラス
/// </summary>
class GeometryObject final {
public:

	GeometryObject() = default;
	~GeometryObject() = default;

	/// <summary>
	/// Geometryを設定する
	/// </summary>
	/// <typeparam name="ShapePolicy">: 型</typeparam>
	/// <typeparam name="...Args">: 可変長引数</typeparam>
	/// <param name="...args">: 型に応じた引数</param>
	template <typename ShapePolicy, typename... Args>
	void Set(Args&&... args) {
		Init();
		GeometryFactory::GetInstance().Create<ShapePolicy>(mesh_.get(), std::forward<Args>(args)...);
	}

	void Update();
	void Draw() const;

private:

	/// <summary>
	/// 初期化は共通して同じ
	/// </summary>
	void Init();

private:

	std::unique_ptr<Mesh> mesh_;
	std::unique_ptr<Material> material_ = nullptr;
	std::unique_ptr<WorldTransform> transform_ = nullptr;

};

