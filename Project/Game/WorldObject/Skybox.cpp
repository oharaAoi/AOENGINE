#include "Skybox.h"
#include "Engine.h"

Skybox::~Skybox() {
}

void Skybox::Init() {

	cube_.Init(Vector3(200,200,200));
	cube_.Inverse();

	// meshの作成
	std::string name = cube_.GetGeometryName();
	if (!ExistMesh(name)) {
		mesh_ = std::make_shared<Mesh>();
		mesh_->Init(GraphicsContext::GetInstance()->GetDevice(), cube_.GetVertex(), cube_.GetIndex());
		AddMeshManager(mesh_, name);
	} else {
		mesh_ = MeshManager::GetInstance()->GetMesh(name);
	}

	// その他の作成
	material_ = Engine::CreateMaterial(Model::ModelMaterialData());
	transform_ = Engine::CreateWorldTransform();
}

void Skybox::Update() {
	transform_->Update();
}

void Skybox::Draw() const {
	Engine::SetPSOObj(Object3dPSO::Skybox);
	ID3D12GraphicsCommandList* commandList = GraphicsContext::GetInstance()->GetCommandList();

	commandList->IASetVertexBuffers(0, 1, &mesh_->GetVBV());
	commandList->IASetIndexBuffer(&mesh_->GetIBV());
	commandList->SetGraphicsRootConstantBufferView(0, material_->GetBufferAdress());

	transform_->BindCommandList(commandList);
	Render::GetInstance()->GetViewProjection()->BindCommandList(commandList);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, useTexture_, 3);

	commandList->DrawIndexedInstanced(mesh_->GetIndexNum(), 1, 0, 0, 0);
}

void Skybox::AddMeshManager(std::shared_ptr<Mesh>& _pMesh, const std::string& name) {
	MeshManager::GetInstance()->AddMesh(GraphicsContext::GetInstance()->GetDevice(), name, name, _pMesh->GetVerticesData(), _pMesh->GetIndices());
}

bool Skybox::ExistMesh(const std::string& name) {
	return MeshManager::GetInstance()->ExistMesh(name);
}
