#include "PBRMaterial.h"
#include "Engine/System/Manager/ImGuiManager.h"

PBRMaterial::PBRMaterial() {}
PBRMaterial::~PBRMaterial() { Finalize(); }

void PBRMaterial::Finalize() {
	materialBuffer_.Reset();
}

void PBRMaterial::Init(ID3D12Device* device) {
	materialBuffer_ = CreateBufferResource(device, sizeof(PBRMaterialData));
	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&pbrMaterial_));

	pbrMaterial_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pbrMaterial_->enableLighting = false;
	pbrMaterial_->uvTransform = Matrix4x4::MakeUnit();

	pbrMaterial_->diffuseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pbrMaterial_->specularColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	pbrMaterial_->roughness = 0.5f;
	pbrMaterial_->metallic = 0.5f;
	pbrMaterial_->shininess = 10;

	uvTranslation_ = {0.0f, 0.0f, 0.0f};
	uvScale_ = { 0.2f, 0.2f, 1.0f };;
	uvRotation_ = { 0.0f, 0.0f, 0.0f };
}

void PBRMaterial::Update() {

}

void PBRMaterial::Draw(ID3D12GraphicsCommandList* commandList) const {
	commandList->SetGraphicsRootConstantBufferView(0, materialBuffer_->GetGPUVirtualAddress());
}

void PBRMaterial::Debug_Gui() {
	if (ImGui::TreeNode("uvTramsform")) {
		if (ImGui::TreeNode("scale")) {
			ImGui::DragFloat4("uvScale", &uvScale_.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("rotation")) {
			ImGui::DragFloat4("uvRotation", &uvRotation_.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("translation")) {
			ImGui::DragFloat4("uvTranslation", &uvTranslation_.x, 0.01f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	ImGui::DragFloat("roughness", &pbrMaterial_->roughness, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("metallic", &pbrMaterial_->metallic, 0.01f, 0.0f, 1.0f);
	//ImGui::DragFloat("shininess", &pbrMaterial_->shininess, 0.1f);
}

void PBRMaterial::SetMaterialData(Model::ModelMaterialData materialData) {
	materials_ = materialData;

	pbrMaterial_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pbrMaterial_->enableLighting = true;
	pbrMaterial_->uvTransform = Matrix4x4::MakeUnit();

	pbrMaterial_->diffuseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pbrMaterial_->specularColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	pbrMaterial_->roughness = 1.0f;
	pbrMaterial_->metallic = 1.0f;
	pbrMaterial_->shininess = 50;
}