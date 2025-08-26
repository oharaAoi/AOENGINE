#include "Material.h"
#include "ImGuiFileDialog.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/Utilities/Loader.h"
#include "Engine/Core/GraphicsContext.h"	

Material::~Material() {
	cBuffer_.Reset();
}

void Material::Init() {
	GraphicsContext* ctx = GraphicsContext::GetInstance();

	// ---------------------------------------------------------------
	// ↓Materialの設定
	// ---------------------------------------------------------------
	cBuffer_ = CreateBufferResource(ctx->GetDevice(), sizeof(MaterialData));
	cBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&material_));

	uvTransform_.scale = CVector3::UNIT;
	uvTransform_.rotate = CVector3::ZERO;
	uvTransform_.translate = CVector3::ZERO;

	color_ = Vector4(1, 1, 1, 1);
	material_->enableLighting = 1;
	material_->color = color_;
	material_->shininess = 1.0f;
	material_->uvTransform = uvTransform_.MakeAffine();
	material_->discardValue = 0.01f;
}

void Material::Update() {
	material_->color = color_;
	material_->uvTransform = uvTransform_.MakeAffine();
}

void Material::SetCommand(ID3D12GraphicsCommandList* commandList) {
	commandList->SetGraphicsRootConstantBufferView(0, cBuffer_->GetGPUVirtualAddress());
}

void Material::Debug_Gui() {
	if (ImGui::CollapsingHeader("Material")) {
		Debug_UV();
		// Textureを変更する
		TextureManager* textureManager = TextureManager::GetInstance();
		textureName_ = textureManager->SelectTexture(textureName_);
		// 色を変更する
		ImGui::Separator();
		ImGui::BulletText("Color");
		ImGui::ColorEdit4("color", &color_.x);
		ImGui::Combo("Lighting", &material_->enableLighting, "None\0Lambert\0HalfLambert");
		ImGui::DragFloat("discard", &material_->discardValue, 0.01f);
	}
}

void Material::SetMaterialData(ModelMaterialData materialData) {
	material_->color = materialData.color;
	material_->enableLighting = true;
	material_->uvTransform = Matrix4x4::MakeUnit();
	material_->shininess = 100;
	material_->discardValue = 0.01f;
	textureName_ = materialData.textureFilePath;
}