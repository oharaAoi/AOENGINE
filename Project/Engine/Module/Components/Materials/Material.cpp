#include "Material.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/Utilities/Loader.h"
#include "Engine/Core/GraphicsContext.h"	

using namespace AOENGINE;

Material::~Material() {
	cBuffer_.Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Material::Init() {
	AOENGINE::GraphicsContext* ctx = AOENGINE::GraphicsContext::GetInstance();

	// ---------------------------------------------------------------
	// ↓Materialの設定
	// ---------------------------------------------------------------
	cBuffer_ = CreateBufferResource(ctx->GetDevice(), sizeof(MaterialData));
	cBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&material_));

	uvTransform_.scale = CVector3::UNIT;
	uvTransform_.rotate = CVector3::ZERO;
	uvTransform_.translate = CVector3::ZERO;

	color_ = AOENGINE::Color::white;
	material_->enableLighting = 1;
	material_->color = color_;
	material_->shininess = 1.0f;
	material_->uvTransform = uvTransform_.MakeAffine();
	material_->discardValue = 0.01f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Material::Update() {
	material_->enableLighting = isLighting_;
	material_->color = color_;
	material_->uvTransform = uvTransform_.MakeAffine();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Material::Debug_Gui() {
	EditShaderType();
	EditUV();
	// Textureを変更する
	TextureManager* textureManager = TextureManager::GetInstance();
	textureName_ = textureManager->SelectTexture(textureName_);
	// 色を変更する
	ImGui::Separator();
	ImGui::BulletText("Color");
	ImGui::ColorEdit4("color", &color_.r);
	ImGui::Combo("Lighting", &material_->enableLighting, "None\0Lambert\0HalfLambert");
	ImGui::DragFloat("discard", &material_->discardValue, 0.01f);
}

void Material::SetMaterialData(ModelMaterialData materialData) {
	material_->color = materialData.color;
	material_->enableLighting = true;
	material_->uvTransform = Math::Matrix4x4::MakeUnit();
	material_->shininess = 100;
	material_->discardValue = 0.01f;
	textureName_ = materialData.textureFilePath;
}