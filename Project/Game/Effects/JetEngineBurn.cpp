#include "JetEngineBurn.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"

void JetEngineBurn::Finalize() {
}

void JetEngineBurn::Init() {
	geometry_.Init(32, 0.1f, 1.8f, 2.0f);
	param_.FromJson(JsonItems::GetData(GetName(), param_.GetName()));
	flareParameter_.FromJson(JsonItems::GetData(GetName(), flareParameter_.GetName()));

	// meshの作成dw 
	std::string name = geometry_.GetGeometryName();
	if (!ExistMesh(name)) {
		mesh_ = std::make_shared<Mesh>();
		mesh_->Init(GraphicsContext::GetInstance()->GetDevice(), geometry_.GetVertex(), geometry_.GetIndex());
		AddMeshManager(mesh_, name);
	} else {
		mesh_ = MeshManager::GetInstance()->GetMesh(name);
	}

	noiseBuffer_ = CreateBufferResource(GraphicsContext::GetInstance()->GetDevice(), sizeof(NoiseUV));
	noiseBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&noiseUV_));

	noiseSRT_.scale = param_.noiseScale;
	noiseSRT_.rotate = CVector3::ZERO;
	noiseSRT_.translate = CVector3::ZERO;
	
	// その他の作成
	material_ = std::make_unique<Material>();
	material_->Init();
	material_->SetColor(param_.color);
	material_->SetAlbedoTexture(param_.materialTexture);

	worldTransform_ = Engine::CreateWorldTransform();
	worldTransform_->SetScale(param_.scale);
	worldTransform_->SetRotate(param_.rotate);
	worldTransform_->SetTranslate(param_.translate);

	noiseAnimation_.Init(&animationValue_, -20.0f, 20.0f, 50.0f, (int)EasingType::None::Liner, LoopType::LOOP);

	// flareの初期化
	enginePos_ = worldTransform_->GetPos();
	flare_ = SceneRenderer::GetInstance()->AddObject<GeometryObject>("jetFlare", "Object_Add.json", 200);
	flare_->Set<PlaneGeometry>(Vector2(8.0f, 8.0f));
	flare_->GetMaterial()->SetAlbedoTexture("renzeFrea.png");
	flare_->GetMaterial()->SetIsLighting(false);
	flare_->GetMaterial()->SetDiscardValue(0.1f);
	flare_->GetTransform()->SetScale(Vector3(2.0f, 2.0f, 2.0f));
	flare_->GetTransform()->SetBillBoard(true);
}

void JetEngineBurn::Update() {
	noiseAnimation_.Update(GameTimer::DeltaTime());
	noiseSRT_.translate.y = animationValue_;
	noiseUV_->uv = noiseSRT_.MakeAffine();

	worldTransform_->Update();
	material_->Update();
}

void JetEngineBurn::PostUpdate() {
	worldTransform_->Update();
	enginePos_ = worldTransform_->GetPos();
	flare_->GetTransform()->SetTranslate(flareParameter_.translate + worldTransform_->GetPos());
	flare_->Update();
}

void JetEngineBurn::Draw() const {
	Engine::SetPipeline(PSOType::Object3d, "Object_TextureBlendAdd.json");
	ID3D12GraphicsCommandList* commandList = GraphicsContext::GetInstance()->GetCommandList();

	commandList->IASetVertexBuffers(0, 1, &mesh_->GetVBV());
	commandList->IASetIndexBuffer(&mesh_->GetIBV());

	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gMaterial");
	commandList->SetGraphicsRootConstantBufferView(index, material_->GetBufferAddress());
	index = pso->GetRootSignatureIndex("gNoiseUV");
	commandList->SetGraphicsRootConstantBufferView(index, noiseBuffer_->GetGPUVirtualAddress());

	index = pso->GetRootSignatureIndex("gWorldTransformMatrix");
	worldTransform_->BindCommandList(commandList, index);
	index = pso->GetRootSignatureIndex("gViewProjectionMatrix");
	Render::GetInstance()->GetViewProjection()->BindCommandList(commandList, index);
	index = pso->GetRootSignatureIndex("gViewProjectionMatrixPrev");
	Render::GetInstance()->GetViewProjection()->BindCommandListPrev(commandList, index);

	index = pso->GetRootSignatureIndex("gTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, material_->GetAlbedoTexture(), index);
	index = pso->GetRootSignatureIndex("gNoiseTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, param_.blendTexture, index);

	commandList->DrawIndexedInstanced(mesh_->GetIndexNum(), 1, 0, 0, 0);
}

void JetEngineBurn::Debug_Gui() {
	if (ImGui::TreeNode("Burn")) {
		worldTransform_->Debug_Gui();
		material_->Debug_Gui();

		param_.color = material_->GetColor();
		param_.scale = worldTransform_->GetScale();
		param_.rotate = worldTransform_->GetRotate();
		param_.translate = worldTransform_->GetTranslate();
		param_.materialTexture = material_->GetAlbedoTexture();

		if (ImGui::CollapsingHeader("BlendTexture")) {
			if (ImGui::TreeNode("UV")) {
				ImGui::DragFloat3("scale", &noiseSRT_.scale.x, 0.1f);
				ImGui::DragFloat3("rotate", &noiseSRT_.rotate.x, 0.1f);
				ImGui::DragFloat3("translate", &noiseSRT_.translate.x, 0.1f);
				ImGui::TreePop();
			}
			param_.blendTexture = TextureManager::GetInstance()->SelectTexture(param_.blendTexture);

			param_.noiseScale = noiseSRT_.scale;
		}

		if (ImGui::Button("Save")) {
			JsonItems::Save(GetName(), param_.ToJson(param_.GetName()));
		}
		if (ImGui::Button("Apply")) {
			param_.FromJson(JsonItems::GetData(GetName(), param_.GetName()));
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Flare")) {
		flare_->Debug_Gui();
		ImGui::DragFloat3("translate", &flareParameter_.translate.x, 0.1f);
		if (ImGui::Button("Save")) {
			JsonItems::Save(GetName(), flareParameter_.ToJson(flareParameter_.GetName()));
		}
		if (ImGui::Button("Apply")) {
			flareParameter_.FromJson(JsonItems::GetData(GetName(), flareParameter_.GetName()));
		}
		ImGui::TreePop();
	}
}

void JetEngineBurn::AddMeshManager(std::shared_ptr<Mesh>& _pMesh, const std::string& name) {
	MeshManager::GetInstance()->AddMesh(GraphicsContext::GetInstance()->GetDevice(), name, name, _pMesh->GetVerticesData(), _pMesh->GetIndices());
}

bool JetEngineBurn::ExistMesh(const std::string& name) {
	return MeshManager::GetInstance()->ExistMesh(name);
}