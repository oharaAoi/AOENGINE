#include "JetEngineBurn.h"
#include "Engine.h"

void JetEngineBurn::Finalize() {
}

void JetEngineBurn::Init() {
	geometry_.Init(Vector2(1.0f, 1.0f), 32);
	
	// meshの作成dw 
	std::string name = geometry_.GetGeometryName();
	if (!ExistMesh(name)) {
		mesh_ = std::make_shared<Mesh>();
		mesh_->Init(GraphicsContext::GetInstance()->GetDevice(), geometry_.GetVertex(), geometry_.GetIndex());
		AddMeshManager(mesh_, name);
	} else {
		mesh_ = MeshManager::GetInstance()->GetMesh(name);
	}

	// その他の作成
	material_ = Engine::CreateMaterial(Model::ModelMaterialData());
	worldTransform_ = Engine::CreateWorldTransform();
	worldTransform_->SetScale(Vector3(0.1f, 10.0f, 0.1f));

	material_->SetUseTexture("gradationLine.png");
}

void JetEngineBurn::Update() {
	worldTransform_->Update();
}

void JetEngineBurn::Draw() const {
	Engine::SetPipeline(PSOType::Object3d, "Object_TextureBlendAdd.json");
	ID3D12GraphicsCommandList* commandList = GraphicsContext::GetInstance()->GetCommandList();

	commandList->IASetVertexBuffers(0, 1, &mesh_->GetVBV());
	commandList->IASetIndexBuffer(&mesh_->GetIBV());

	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gMaterial");
	commandList->SetGraphicsRootConstantBufferView(index, material_->GetBufferAdress());

	index = pso->GetRootSignatureIndex("gWorldTransformMatrix");
	worldTransform_->BindCommandList(commandList, index);
	index = pso->GetRootSignatureIndex("gViewProjectionMatrix");
	Render::GetInstance()->GetViewProjection()->BindCommandList(commandList, index);

	index = pso->GetRootSignatureIndex("gTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, material_->GetUseTexture(), index);
	index = pso->GetRootSignatureIndex("gNoiseTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, "white.png", index);

	commandList->DrawIndexedInstanced(mesh_->GetIndexNum(), 1, 0, 0, 0);
}

#ifdef _DEBUG
void JetEngineBurn::Debug_Gui() {
	worldTransform_->Debug_Gui();
	material_->ImGuiDraw();
}
#endif // _DEBUG

void JetEngineBurn::AddMeshManager(std::shared_ptr<Mesh>& _pMesh, const std::string& name) {
	MeshManager::GetInstance()->AddMesh(GraphicsContext::GetInstance()->GetDevice(), name, name, _pMesh->GetVerticesData(), _pMesh->GetIndices());
}

bool JetEngineBurn::ExistMesh(const std::string& name) {
	return MeshManager::GetInstance()->ExistMesh(name);
}