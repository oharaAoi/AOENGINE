#include "GeometryManager.h"
#include "Engine/Engine.h"
#include "Engine/Geometry/Polygon/PlaneGeometry.h"
#include "Engine/Geometry/Polygon/SphereGeometry.h"
#include "Engine/Geometry/Polygon/CubeGeometry.h"
#include "Engine/Geometry/Polygon/RingGeometry.h"
#include "Engine/Geometry/Polygon/CylinderGeometry.h"

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

void GeometryManager::SetSphere(Mesh* _pMesh, const Vector2& size, uint32_t division) {
	SphereGeometry sphere;
	sphere.Init(size, division);
	_pMesh->Init(Engine::GetDevice(), sphere.GetVertex(), sphere.GetIndex());
}

void GeometryManager::SetCube(Mesh* _pMesh, const Vector3& size) {
	CubeGeometry cube;
	cube.Init(size);
	_pMesh->Init(Engine::GetDevice(), cube.GetVertex(), cube.GetIndex());
}

void GeometryManager::SetRing(Mesh* _pMesh, uint32_t division, float outerRadius, float innerRadius) {
	RingGeometry ring;
	ring.Init(division, outerRadius, innerRadius);
	_pMesh->Init(Engine::GetDevice(), ring.GetVertex(), ring.GetIndex());
}

void GeometryManager::SetCylinder(Mesh* _pMesh, uint32_t division, float radius, float height) {
	CylinderGeometry cylinder;
	cylinder.Init(division, radius, height);
	_pMesh->Init(Engine::GetDevice(), cylinder.GetVertex(), cylinder.GetIndex());
}
