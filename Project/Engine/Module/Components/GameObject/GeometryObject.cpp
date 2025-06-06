#include "GeometryObject.h"
#include "Engine/Render.h"
#include "Engine/Engine.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/Module/Geometry/Polygon/PlaneGeometry.h"
#include "Engine/Module/Geometry/Polygon/SphereGeometry.h"
#include "Engine/Module/Geometry/Polygon/CubeGeometry.h"
#include "Engine/Module/Geometry/Polygon/RingGeometry.h"
#include "Engine/Module/Geometry/Polygon/CylinderGeometry.h"
#include "Engine/Module/Components/GameObject/Model.h"

GeometryObject::~GeometryObject() {
	material_->Finalize();
	transform_->Finalize();
}

void GeometryObject::Update() {
	transform_->Update();
}

void GeometryObject::Draw() const {

	ID3D12GraphicsCommandList* commandList = GraphicsContext::GetInstance()->GetCommandList();
	Pipeline* pso = Engine::GetLastUsedPipeline();

	Render::DrawLightGroup(pso);
	commandList->IASetVertexBuffers(0, 1, &mesh_->GetVBV());
	commandList->IASetIndexBuffer(&mesh_->GetIBV());

	UINT index = pso->GetRootSignatureIndex("gMaterial");
	commandList->SetGraphicsRootConstantBufferView(index, material_->GetBufferAdress());

	index = pso->GetRootSignatureIndex("gWorldTransformMatrix");
	transform_->BindCommandList(commandList, index);

	index = pso->GetRootSignatureIndex("gViewProjectionMatrix");
	Render::GetInstance()->GetViewProjection()->BindCommandList(commandList, index);

	index = pso->GetRootSignatureIndex("gTexture");
	std::string textureName = material_->GetUseTexture();
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureName, index);
	
	commandList->DrawIndexedInstanced(mesh_->GetIndexNum(), 1, 0, 0, 0);
}


#ifdef _DEBUG
void GeometryObject::Debug_Gui() {
	material_->ImGuiDraw();
}
#endif

void GeometryObject::SetEditorWindow() {
#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, id_);
#endif // _DEBUG
}

void GeometryObject::Init() {
	mesh_ = std::make_unique<Mesh>();
	material_ = Engine::CreateMaterial(Model::ModelMaterialData());
	transform_ = Engine::CreateWorldTransform();
}