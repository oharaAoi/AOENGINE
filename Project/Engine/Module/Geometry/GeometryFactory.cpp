#include "GeometryFactory.h"
#include "Engine/Engine.h"

using namespace AOENGINE;

GeometryFactory& GeometryFactory::GetInstance() {
	static GeometryFactory instance;
	return instance;
}

void GeometryFactory::Init() {
	pDevice_ = AOENGINE::GraphicsContext::GetInstance()->GetDevice();
}

void GeometryFactory::SetMesh(std::shared_ptr<Mesh>& _pMesh, const std::string& name) {
	_pMesh = AOENGINE::MeshManager::GetInstance()->GetMesh(name);
}

void GeometryFactory::AddMeshManager(std::shared_ptr<Mesh>& _pMesh, const std::string& name) {
	AOENGINE::MeshManager::GetInstance()->AddMesh(pDevice_, name, name, _pMesh->GetVerticesData(), _pMesh->GetIndices());
}

bool GeometryFactory::ExistMesh(const std::string& name) {
	return AOENGINE::MeshManager::GetInstance()->ExistMesh(name);
}
