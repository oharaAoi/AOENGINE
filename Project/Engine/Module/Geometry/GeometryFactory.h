#pragma once
#include <utility>
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Geometry/Polygon/IGeometry.h"
#include "Engine/System/Manager/MeshManager.h"

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
	void Create(std::shared_ptr<Mesh>& _pMesh, Args&&... args) {
		createMesh_ = false;
		ShapePolicy geometry;
		geometry.Init(std::forward<Args>(args)...);
		std::string name = geometry.GetGeometryName();
		if (!ExistMesh(name)) {
			_pMesh = std::make_shared<Mesh>();
			_pMesh->Init(pDevice_, geometry.GetVertex(), geometry.GetIndex());
			AddMeshManager(_pMesh, name);
		} else {
			_pMesh = MeshManager::GetInstance()->GetMesh(name);
		}
	}

private:

	void SetMesh(std::shared_ptr<Mesh>& _pMesh, const std::string& name);

	void AddMeshManager(std::shared_ptr<Mesh>& _pMesh, const std::string& name);

	bool ExistMesh(const std::string& name);

private:

	ID3D12Device* pDevice_ = nullptr;

	bool createMesh_;

};

