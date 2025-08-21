#include "PBRMaterial.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Core/GraphicsContext.h"

PBRMaterial::PBRMaterial() {}
PBRMaterial::~PBRMaterial() { cBuffer_.Reset(); }


void PBRMaterial::Init() {
	GraphicsContext* ctx = GraphicsContext::GetInstance();

	cBuffer_ = CreateBufferResource(ctx->GetDevice(), sizeof(PBRMaterialData));
	cBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&pbrMaterial_));

	pbrMaterial_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pbrMaterial_->enableLighting = false;
	pbrMaterial_->uvTransform = Matrix4x4::MakeUnit();

	pbrMaterial_->diffuseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pbrMaterial_->specularColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	pbrMaterial_->roughness = 0.5f;
	pbrMaterial_->metallic = 0.5f;
	pbrMaterial_->shininess = 10;

	uvTransform_.scale = CVector3::UNIT;
	uvTransform_.rotate = CVector3::ZERO;
	uvTransform_.translate = CVector3::ZERO;
	pbrMaterial_->uvTransform= uvTransform_.MakeAffine();
}

void PBRMaterial::Update() {
	pbrMaterial_->uvTransform = uvTransform_.MakeAffine();
}

void PBRMaterial::SetCommand(ID3D12GraphicsCommandList* commandList) {
	commandList->SetGraphicsRootConstantBufferView(0, cBuffer_->GetGPUVirtualAddress());
}

void PBRMaterial::Debug_Gui() {
	Debug_UV();
	ImGui::DragFloat("roughness", &pbrMaterial_->roughness, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("metallic", &pbrMaterial_->metallic, 0.01f, 0.0f, 1.0f);
}

void PBRMaterial::SetMaterialData(ModelMaterialData materialData) {
	textureName_ = materialData.textureFilePath;

	pbrMaterial_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pbrMaterial_->enableLighting = true;
	pbrMaterial_->uvTransform = Matrix4x4::MakeUnit();

	pbrMaterial_->diffuseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pbrMaterial_->specularColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	pbrMaterial_->roughness = 1.0f;
	pbrMaterial_->metallic = 1.0f;
	pbrMaterial_->shininess = 50;
}