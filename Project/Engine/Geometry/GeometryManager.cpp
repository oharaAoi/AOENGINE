#include "GeometryManager.h"
#include "Engine/Engine.h"

GeometryManager& GeometryManager::GetInstance() {
	static GeometryManager instance;
	return instance;
}

void GeometryManager::Init() {
	
}

void GeometryManager::SetPlane(Mesh* _pMesh, const Vector2& size) {
	PlaneGeometry plane;
	plane.Init(size);
	_pMesh->Init(Engine::GetDevice(), plane.GetVertex(), plane.GetIndex());
}
