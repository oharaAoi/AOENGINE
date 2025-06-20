#include "PulseArmor.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/Core/GraphicsContext.h"
#include "Game/Information/ColliderCategory.h"
#include "Engine/System/Editer/Window/EditorWindows.h"

PulseArmor::~PulseArmor() {
	
}

void PulseArmor::Init() {
	SetName("PulseArmor");
	geometry_.Init(CVector2::UNIT, 16, "armor");

	std::string name = geometry_.GetGeometryName();
	if (!MeshManager::GetInstance()->ExistMesh(name)) {
		mesh_ = std::make_shared<Mesh>();
		mesh_->Init(GraphicsContext::GetInstance()->GetDevice(), geometry_.GetVertex(), geometry_.GetIndex());
		MeshManager::GetInstance()->AddMesh(GraphicsContext::GetInstance()->GetDevice(), name, name, mesh_->GetVerticesData(), mesh_->GetIndices());
	} else {
		mesh_ = MeshManager::GetInstance()->GetMesh(name);
	}
	
	material_ = Engine::CreateMaterial(Model::ModelMaterialData());
	worldTransform_ = Engine::CreateWorldTransform();
	material_->SetUseTexture("white.png");

	GraphicsContext* graphicsCtx = GraphicsContext::GetInstance();
	settingBuffer_ = std::make_unique<DxResource>();
	settingBuffer_->Init(graphicsCtx->GetDevice(), graphicsCtx->GetDxHeap(), ResourceType::COMMON);
	settingBuffer_->CreateResource(sizeof(DissolveSetting));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	uvSrt_.scale = CVector3::UNIT;
	uvSrt_.rotate = CVector3::ZERO;
	uvSrt_.translate = CVector3::ZERO;

	setting_->uvTransform = uvSrt_.MakeAffine();
	setting_->color = Vector4(CVector3::UNIT, 1.0f);
	setting_->edgeColor = Vector4(CVector3::UNIT, 1.0f);
	setting_->threshold = 0.02f;

	noiseTexture_ = "noise1.png";

#ifdef _DEBUG
	EditorWindows::AddObjectWindow(this, GetName());
#endif // _DEBUG
}

void PulseArmor::Update() {
	setting_->uvTransform = uvSrt_.MakeAffine();
	worldTransform_->Update();
}

void PulseArmor::Draw() const {
	Engine::SetPipeline(PSOType::Object3d, "Object_Dissolve.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	ID3D12GraphicsCommandList* commandList = GraphicsContext::GetInstance()->GetCommandList();

	// VS
	commandList->IASetVertexBuffers(0, 1, &mesh_->GetVBV());
	commandList->IASetIndexBuffer(&mesh_->GetIBV());
	
	UINT index = pso->GetRootSignatureIndex("gWorldTransformMatrix");
	worldTransform_->BindCommandList(commandList, index);
	index = pso->GetRootSignatureIndex("gViewProjectionMatrix");
	Render::GetInstance()->GetViewProjection()->BindCommandList(commandList, index);

	// PS
	index = pso->GetRootSignatureIndex("gSetting");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	index = pso->GetRootSignatureIndex("gTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, material_->GetUseTexture(), index);
	index = pso->GetRootSignatureIndex("gMaskTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, noiseTexture_, index);

	commandList->DrawIndexedInstanced(mesh_->GetIndexNum(), 1, 0, 0, 0);
}

#ifdef _DEBUG
void PulseArmor::Debug_Gui() {
	worldTransform_->Debug_Gui();
	material_->ImGuiDraw();

	if (ImGui::CollapsingHeader("Setting")) {
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;// | ImGuiTreeNodeFlags_Framed;
		if (ImGui::TreeNodeEx("uvTransform", flags)) {
			if (ImGui::TreeNode("scale")) {
				ImGui::DragFloat4("uvScale", &uvSrt_.scale.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("rotation")) {
				ImGui::DragFloat4("uvRotation", &uvSrt_.rotate.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("translation")) {
				ImGui::DragFloat4("uvTranslation", &uvSrt_.translate.x, 0.01f);
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
		ImGui::ColorEdit4("color", &setting_->color.x);
		ImGui::ColorEdit4("edgeColor", &setting_->edgeColor.x);
		ImGui::DragFloat("threshold", &setting_->threshold, 0.01f);

		noiseTexture_ = TextureManager::GetInstance()->SelectTexture(noiseTexture_);

		setting_->threshold = std::clamp(setting_->threshold, 0.0f, 1.0f);
	}
}
#endif