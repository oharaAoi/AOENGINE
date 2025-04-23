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
		createMesh_ = false;
		ShapePolicy geometry;
		geometry.Init(std::forward<Args>(args)...);
		std::string name = geometry.GetGeometryName();
		if (!ExistMesh(name)) {
			_pMesh->Init(pDevice_, geometry.GetVertex(), geometry.GetIndex());
			AddMeshManager(_pMesh, name);
		} else {
			SetMesh(_pMesh, name);
		}
	}

private:

	void SetMesh(Mesh* _pMesh, const std::string& name);

	void AddMeshManager(Mesh* _pMesh, const std::string& name);

	bool ExistMesh(const std::string& name);

private:

	ID3D12Device* pDevice_ = nullptr;

	bool createMesh_;

};

