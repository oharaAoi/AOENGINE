#include "HitExploade.h"
#include "Engine/Render/SceneRenderer.h"

HitExplode::~HitExplode() {
	Finalize();
}

void HitExplode::Finalize() {
	material_->Finalize();;
	worldTransform_->Finalize();
	BaseGameObject::Finalize();
}

void HitExplode::Init() {
	SetName("HitExplode");
	geometry_.Init();

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
	worldTransform_->SetScale(Vector3(7.0f, 7.0f, 7.0f));
	worldTransform_->rotation_ = Quaternion::AngleAxis(kPI, CVector3::UP);
	
	material_->SetUseTexture("image.png");

	Vector2 uvScale = Vector2(1.0f / columns, 1.0f / rows);
	material_->SetUvScale(Vector3(uvScale.x, uvScale.y, 0.1f));

	/*if (blendMode_ == 1) {
		SceneRenderer::GetInstance()->SetObject(Object3dPSO::TextureBlendNone, this);

	} else if(blendMode_ == 2) {
		SceneRenderer::GetInstance()->SetObject(Object3dPSO::TextureBlendNormal, this);

	} else if (blendMode_ == 3) {
		SceneRenderer::GetInstance()->SetObject(Object3dPSO::TextureBlendAdd, this);
	}
	*/
	currentTimer_ = 0.0f;
	frameRate_ = 40.0f;

	isAlive_ = true;

}

void HitExplode::Update() {
	currentTimer_ += GameTimer::DeltaTime() * frameRate_;
	float t = currentTimer_ / 30.0f;
	material_->SetAlpha(1.0f - t);
	if ((int)currentTimer_ >= 30) {
		isAlive_ = false;
		currentTimer_ = 0;
	}

	int col = (int)currentTimer_ % (int)columns;
	int row = (int)currentTimer_ / (int)rows;

	Vector2 uvScale = Vector2(1.0f / columns, 1.0f / rows);
	Vector2 uvOffset = Vector2(
		col * uvScale.x,
		row * uvScale.y
	);

	
	material_->SetUvTranslation(Vector3(uvOffset.x, uvOffset.y, 0.0f));

	Vector3 directionToCamera = worldTransform_->translate_ - Render::GetEyePos();
	Quaternion lookRotation = Quaternion::LookRotation(directionToCamera);
	worldTransform_->rotation_ = Quaternion::AngleAxis(kPI, lookRotation.MakeUp()) * lookRotation;
	worldTransform_->Update();
	material_->Update(Matrix4x4::MakeUnit());
}

void HitExplode::Draw() const {
	if (!isAlive_) { return; }

	if (blendMode_ == 1) {
		Engine::SetPSOObj(Object3dPSO::TextureBlendNone);
	} else if (blendMode_ == 2) {
		Engine::SetPSOObj(Object3dPSO::TextureBlendNormal);

	} else if (blendMode_ == 3) {
		Engine::SetPSOObj(Object3dPSO::TextureBlendAdd);
	}

	ID3D12GraphicsCommandList* commandList = GraphicsContext::GetInstance()->GetCommandList();

	commandList->IASetVertexBuffers(0, 1, &mesh_->GetVBV());
	commandList->IASetIndexBuffer(&mesh_->GetIBV());
	commandList->SetGraphicsRootConstantBufferView(0, material_->GetBufferAdress());

	worldTransform_->BindCommandList(commandList);
	Render::GetInstance()->GetViewProjection()->BindCommandList(commandList);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, material_->GetUseTexture(), 3);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, "FireNoize.png", 4);

	commandList->DrawIndexedInstanced(mesh_->GetIndexNum(), 1, 0, 0, 0);
}

void HitExplode::Set(const Vector3& pos, const Vector4& color, const std::string& useTexture) {
	material_->SetColor(color);
	worldTransform_->translate_ = pos;
	material_->SetUseTexture(useTexture);
}

#ifdef _DEBUG
void HitExplode::Debug_Gui() {
	ImGui::DragInt("index", &frameIndex);
	ImGui::DragFloat("frameRate", &frameRate_);
	frameIndex = std::clamp(frameIndex, 0, 30);
	ImGui::SameLine();
	if (ImGui::Button("-")) {
		frameIndex--;
	}
	ImGui::SameLine();
	if (ImGui::Button("+")) {
		frameIndex++;
	}
	ImGui::Separator();
	worldTransform_->Debug_Gui();
	material_->ImGuiDraw();
}
#endif // _DEBUG

void HitExplode::AddMeshManager(std::shared_ptr<Mesh>& _pMesh, const std::string& name) {
	MeshManager::GetInstance()->AddMesh(GraphicsContext::GetInstance()->GetDevice(), name, name, _pMesh->GetVerticesData(), _pMesh->GetIndices());
}

bool HitExplode::ExistMesh(const std::string& name) {
	return MeshManager::GetInstance()->ExistMesh(name);
}
