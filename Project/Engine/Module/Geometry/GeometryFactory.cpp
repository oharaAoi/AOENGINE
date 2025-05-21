#include "GeometryFactory.h"
#include "Engine/Engine.h"

GeometryFactory& GeometryFactory::GetInstance() {
	static GeometryFactory instance;
	return instance;
}

void GeometryFactory::Init() {
	pDevice_ = GraphicsContext::GetInstance()->GetDevice();
}

void GeometryFactory::SetMesh(std::shared_ptr<Mesh>& _pMesh, const std::string& name) {
	_pMesh = MeshManager::GetInstance()->GetMesh(name);
}

void GeometryFactory::AddMeshManager(std::shared_ptr<Mesh>& _pMesh, const std::string& name) {
	MeshManager::GetInstance()->AddMesh(pDevice_, name, name, _pMesh->GetVerticesData(), _pMesh->GetIndices());
}

bool GeometryFactory::ExistMesh(const std::string& name) {
	return MeshManager::GetInstance()->ExistMesh(name);
}
