#include "HitExploade.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Engine.h"

HitExplode::~HitExplode() {
	Finalize();
}

void HitExplode::Finalize() {
	material_.reset();;
	worldTransform_->Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

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
	material_ = std::make_unique<Material>();
	material_->Init();
	worldTransform_ = Engine::CreateWorldTransform();
	worldTransform_->SetScale(Vector3(7.0f, 7.0f, 7.0f));
	worldTransform_->srt_.rotate = Quaternion::AngleAxis(kPI, CVector3::UP);
	
	material_->SetAlbedoTexture("image.png");

	Vector2 uvScale = Vector2(1.0f / columns, 1.0f / rows);
	material_->SetUvScale(Vector3(uvScale.x, uvScale.y, 0.1f));

	currentTimer_ = 0.0f;
	frameRate_ = 40.0f;

	isAlive_ = true;


}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

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

	
	material_->SetUvTranslate(Vector3(uvOffset.x, uvOffset.y, 0.0f));

	Vector3 directionToCamera = worldTransform_->srt_.translate - Render::GetEyePos();
	Quaternion lookRotation = Quaternion::LookRotation(directionToCamera);
	worldTransform_->srt_.rotate = Quaternion::AngleAxis(kPI, lookRotation.MakeUp()) * lookRotation;
	worldTransform_->Update();
	material_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void HitExplode::PreDraw() const {

}

void HitExplode::Draw() const {
	if (!isAlive_) { return; }

	if (blendMode_ == 1) {
		Engine::SetPipeline(PSOType::Object3d, "Object_TextureBlendNone.json");
	} else if (blendMode_ == 2) {
		Engine::SetPipeline(PSOType::Object3d, "Object_TextureBlendNormal.json");
	} else if (blendMode_ == 3) {
		Engine::SetPipeline(PSOType::Object3d, "Object_TextureBlendAdd.json");
	}

	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gMaterial");

	ID3D12GraphicsCommandList* commandList = GraphicsContext::GetInstance()->GetCommandList();

	commandList->IASetVertexBuffers(0, 1, &mesh_->GetVBV());
	commandList->IASetIndexBuffer(&mesh_->GetIBV());
	commandList->SetGraphicsRootConstantBufferView(index, material_->GetBufferAddress());

	index = pso->GetRootSignatureIndex("gWorldTransformMatrix");
	worldTransform_->BindCommandList(commandList, index);
	index = pso->GetRootSignatureIndex("gViewProjectionMatrix");
	Render::GetInstance()->GetViewProjection()->BindCommandList(commandList, index);
	index = pso->GetRootSignatureIndex("gViewProjectionMatrixPrev");
	Render::GetInstance()->GetViewProjection()->BindCommandListPrev(commandList, index);

	index = pso->GetRootSignatureIndex("gTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, material_->GetAlbedoTexture(), index);
	index = pso->GetRootSignatureIndex("gNoiseTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, "FireNoize.png", index);

	commandList->DrawIndexedInstanced(mesh_->GetIndexNum(), 1, 0, 0, 0);
}

void HitExplode::Set(const Vector3& pos, const Color& color, const std::string& useTexture) {
	material_->SetColor(color);
	worldTransform_->srt_.translate = pos;
	material_->SetAlbedoTexture(useTexture);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

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
	material_->Debug_Gui();
}

void HitExplode::AddMeshManager(std::shared_ptr<Mesh>& _pMesh, const std::string& name) {
	MeshManager::GetInstance()->AddMesh(GraphicsContext::GetInstance()->GetDevice(), name, name, _pMesh->GetVerticesData(), _pMesh->GetIndices());
}

bool HitExplode::ExistMesh(const std::string& name) {
	return MeshManager::GetInstance()->ExistMesh(name);
}
