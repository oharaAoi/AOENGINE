#include "JetEngineBurn.h"
#include "Engine.h"
#include "Engine/Lib/GameTimer.h"

void JetEngineBurn::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngineBurn::Init() {
	geometry_.Init(Vector2(1,1));
	param_.Load();
	
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

	initScale_ = param_.scale;
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

	noiseAnimation_.Init(-20.0f, 20.0f, 50.0f, (int)EasingType::None::Liner, LoopType::Loop);

	exeTime_ = 0.5f;
	onOffTime_ = 0;
	boostOn_ = false;

	param_.scale = Vector3::Lerp(CVector3::ZERO, initScale_, 0);
	worldTransform_->SetScale(param_.scale);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngineBurn::Update() {
	noiseAnimation_.Update(GameTimer::DeltaTime());
	noiseSRT_.translate.y = noiseAnimation_.GetValue();
	noiseUV_->uv = noiseSRT_.MakeAffine();

	worldTransform_->Update();
	material_->Update();

	if (boostOn_) {
		if (onOffTime_ < exeTime_) {
			onOffTime_ += GameTimer::DeltaTime();
			float t = onOffTime_ / exeTime_;
			param_.scale = Vector3::Lerp(CVector3::ZERO, initScale_, t);
			
			worldTransform_->SetScale(param_.scale);
		}
	} else {
		if (onOffTime_ >  0.0f) {
			onOffTime_ -= GameTimer::DeltaTime();
			float t = onOffTime_ / exeTime_;
			param_.scale = Vector3::Lerp(CVector3::ZERO, initScale_, t);
	
			worldTransform_->SetScale(param_.scale);
		}
	}
}

void JetEngineBurn::PostUpdate() {
	worldTransform_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngineBurn::Debug_Gui() {
	if (ImGui::TreeNode("Burn")) {
		static bool isDebug = false;
		ImGui::Checkbox("isDebug", &isDebug);
		boostOn_ = isDebug;
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

		param_.SaveAndLoad();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Mesh")) {
		geometry_.Debug_Gui();
		mesh_->SetVertexData(geometry_.GetVertex());
		ImGui::TreePop();
	}
}


void JetEngineBurn::Parameter::Debug_Gui() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ meshの処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngineBurn::AddMeshManager(std::shared_ptr<Mesh>& _pMesh, const std::string& name) {
	MeshManager::GetInstance()->AddMesh(GraphicsContext::GetInstance()->GetDevice(), name, name, _pMesh->GetVerticesData(), _pMesh->GetIndices());
}

bool JetEngineBurn::ExistMesh(const std::string& name) {
	return MeshManager::GetInstance()->ExistMesh(name);
}

void JetEngineBurn::BoostOn() {
	boostOn_ = true;
}

void JetEngineBurn::BoostOff() {
	boostOn_ = false;
}
