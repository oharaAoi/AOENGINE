#include "GeometryObject.h"
#include "Engine/Render.h"
#include "Engine/Engine.h"
#include "Engine/Geometry/Polygon/PlaneGeometry.h"
#include "Engine/Geometry/Polygon/SphereGeometry.h"
#include "Engine/Geometry/Polygon/CubeGeometry.h"
#include "Engine/Geometry/Polygon/RingGeometry.h"
#include "Engine/Geometry/Polygon/CylinderGeometry.h"
#include "Engine/Components/GameObject/Model.h"

//void GeometryObject::SetPlane(const Vector2& size) {
//	Init();
//	GeometryFactory::GetInstance().Create<PlaneGeometry>(mesh_.get(), size);
//}
//
//void GeometryObject::SetSphere(const Vector2& size, uint32_t division) {
//	Init();
//	GeometryFactory::GetInstance().Create<SphereGeometry>(mesh_.get(), size, division);
//}
//
//void GeometryObject::SetCube(const Vector3& size) {
//	Init();
//	GeometryFactory::GetInstance().Create<CubeGeometry>(mesh_.get(), size);
//}
//
//void GeometryObject::SetRing(uint32_t division, float outerRadius, float innerRadius) {
//	Init();
//	GeometryFactory::GetInstance().Create<RingGeometry>(mesh_.get(), division, outerRadius, innerRadius);
//}
//
//void GeometryObject::SetCylinder(uint32_t division, float radius, float height) {
//	Init();
//	GeometryFactory::GetInstance().Create<CylinderGeometry>(mesh_.get(), division, radius, height);
//}

void GeometryObject::Update() {
	transform_->Update();
}

void GeometryObject::Draw() const {
	ID3D12GraphicsCommandList* commandList = Engine::GetCommandList();

	commandList->IASetVertexBuffers(0, 1, &mesh_->GetVBV());
	commandList->IASetIndexBuffer(&mesh_->GetIBV());
	commandList->SetGraphicsRootConstantBufferView(0, material_->GetBufferAdress());

	transform_->BindCommandList(commandList);
	Render::GetInstance()->GetViewProjection()->BindCommandList(commandList);

	std::string textureName = material_->GetUseTexture();
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureName, 3);

	commandList->DrawIndexedInstanced(mesh_->GetIndexNum(), 1, 0, 0, 0);
}

void GeometryObject::Init() {
	mesh_ = std::make_unique<Mesh>();
	material_ = Engine::CreateMaterial(Model::ModelMaterialData());
	transform_ = Engine::CreateWorldTransform();
}