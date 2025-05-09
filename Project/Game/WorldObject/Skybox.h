#pragma once
#include <memory>
#include <string>
#include "Engine/Module/Geometry/Polygon/CubeGeometry.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// skybox
/// </summary>
class Skybox final {
public:

	Skybox() = default;
	~Skybox();

	void Init();

	void Update();

	void Draw() const;

public:

	void AddMeshManager(std::shared_ptr<Mesh>& _pMesh, const std::string& name);

	bool ExistMesh(const std::string& name);

private:

	CubeGeometry cube_;

	std::shared_ptr<Mesh> mesh_;
	std::unique_ptr<Material> material_ = nullptr;
	std::unique_ptr<WorldTransform> transform_ = nullptr;

	std::string useTexture_ = "rostock_laage_airport_4k.dds";
};

