#pragma once
#include <memory>
#include <string>
#include "Engine/Module/Geometry/Polygon/CylinderGeometry.h"
#include "Engine/Module/Geometry/Polygon/SphereGeometry.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

class JetEngineBurn {
public:

	JetEngineBurn() = default;
	~JetEngineBurn() = default;

	void Finalize() ;
	void Init();
	void Update();
	void Draw() const;

	void Debug_Gui();

public:

	void AddMeshManager(std::shared_ptr<Mesh>& _pMesh, const std::string& name);

	bool ExistMesh(const std::string& name);

	WorldTransform* GetWorldTransform() { return worldTransform_.get(); }

private:

	SphereGeometry geometry_;

	std::shared_ptr<Mesh> mesh_;
	std::unique_ptr<Material> material_ = nullptr;
	std::unique_ptr<WorldTransform> worldTransform_ = nullptr;

};

