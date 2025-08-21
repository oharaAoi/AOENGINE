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
	material_ = std::make_unique<Material>();
	material_->Init();
	transform_ = Engine::CreateWorldTransform();
}

void Skybox::Update() {
	transform_->Update();
}

void Skybox::Draw() const {
	ID3D12GraphicsCommandList* commandList = GraphicsContext::GetInstance()->GetCommandList();
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gMaterial");

	commandList->IASetVertexBuffers(0, 1, &mesh_->GetVBV());
	commandList->IASetIndexBuffer(&mesh_->GetIBV());
	commandList->SetGraphicsRootConstantBufferView(index, material_->GetBufferAddress());

	index = pso->GetRootSignatureIndex("gWorldTransformMatrix");
	transform_->BindCommandList(commandList, index);
	index = pso->GetRootSignatureIndex("gViewProjectionMatrix");
	Render::GetInstance()->GetViewProjection()->BindCommandList(commandList, index);
	index = pso->GetRootSignatureIndex("gViewProjectionMatrixPrev");
	Render::GetInstance()->GetViewProjection()->BindCommandListPrev(commandList, index);

	index = pso->GetRootSignatureIndex("gTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, useTexture_, index);

	commandList->DrawIndexedInstanced(mesh_->GetIndexNum(), 1, 0, 0, 0);
}

void Skybox::AddMeshManager(std::shared_ptr<Mesh>& _pMesh, const std::string& name) {
	MeshManager::GetInstance()->AddMesh(GraphicsContext::GetInstance()->GetDevice(), name, name, _pMesh->GetVerticesData(), _pMesh->GetIndices());
}

bool Skybox::ExistMesh(const std::string& name) {
	return MeshManager::GetInstance()->ExistMesh(name);
}
