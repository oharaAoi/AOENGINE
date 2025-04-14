#pragma once
#include <utility>
#include "Engine/Components/Meshes/Mesh.h"
#include "Engine/Geometry/Polygon/IGeometry.h"

/// <summary>
/// 各Geometryを管理しているクラス
/// </summary>
class GeometryFactory final {
public:

	GeometryFactory() = default; // コンストラクタをprivateに（シングルトン）
	GeometryFactory(const GeometryFactory&) = delete;
	GeometryFactory& operator=(const GeometryFactory&) = delete;

	static GeometryFactory& GetInstance();

	void Init();

	/// <summary>
	/// Geometryを作成する
	/// </summary>
	/// <typeparam name="ShapePolicy">: 型</typeparam>
	/// <typeparam name="...Args">: 可変長引数</typeparam>
	/// <param name="_pMesh">: meshのポインタ</param>
	/// <param name="...args">: 可変長引数</param>
	template <typename ShapePolicy, typename... Args>
	void Create(Mesh* _pMesh, Args&&... args) {
		ShapePolicy geometry;
		geometry.Init(std::forward<Args>(args)...);
		_pMesh->Init(pDevice_, geometry.GetVertex(), geometry.GetIndex());
	}

private:

	ID3D12Device* pDevice_ = nullptr;

};

