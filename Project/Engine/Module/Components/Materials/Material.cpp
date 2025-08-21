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
}

void Material::Update() {
	material_->color = color_;
	material_->uvTransform = uvTransform_.MakeAffine();
}

void Material::SetCommand(ID3D12GraphicsCommandList* commandList) {
	commandList->SetGraphicsRootConstantBufferView(0, cBuffer_->GetGPUVirtualAddress());
}

void Material::Debug_Gui() {

	Debug_UV();
	// Textureを変更する
	SelectTexture();
	// 色を変更する
	ImGui::Separator();
	ImGui::BulletText("Color");
	ImGui::ColorEdit4("color", &color_.x);
	ImGui::Combo("Lighting", &material_->enableLighting, "None\0Lambert\0HalfLambert");

}
void Material::SelectTexture() {
	TextureManager* textureManager = TextureManager::GetInstance();
	static std::string selectedFilename;
	static int selectedIndex = -1; // 選択されたインデックス

	ImGui::Separator();
	ImGui::BulletText("TextureView");

	// -------------------------------------------------
	// ↓ 現在のTextureのViewを表示
	// -------------------------------------------------
	auto currentHandle = textureManager->GetDxHeapHandles(textureName_);
	ImTextureID currentTexID = (ImTextureID)(intptr_t)(currentHandle.handleGPU.ptr);
	ImGui::SetNextWindowBgAlpha(0.85f);
	ImGui::Image(currentTexID, ImVec2(128, 128));

	// -------------------------------------------------
	// ↓ 選択できるTextureのViewｗを表示
	// -------------------------------------------------
	if (ImGui::TreeNode("Files")) {
		ImGui::Text(selectedFilename.c_str());
		ImGui::SameLine();
		if (ImGui::Button("OK")) {
			SetAlbedoTexture(selectedFilename.c_str());
		}
		// ListBox を手動で構築（ListBoxHeader + Selectable）
		if (ImGui::BeginListBox("TextureList")) {
			for (int i = 0; i < textureManager->GetFileNames().size(); ++i) {
				std::string textureName = textureManager->GetFileNames()[i];
				const char* ext = GetFileExtension(textureName.c_str());
				std::string extension(ext);
				// 拡張子で判別する
				if ((extension == "png") || (extension == "jpeg")) {

					const bool isSelected = (i == selectedIndex);
					if (ImGui::Selectable(textureName.c_str(), isSelected)) {
						selectedIndex = i;
						selectedFilename = textureManager->GetFileNames()[i];
					}
					// ホバー中のファイルにプレビューを表示
					if (ImGui::IsItemHovered()) {
						auto handle = textureManager->GetDxHeapHandles(textureName);
						ImTextureID texID = (ImTextureID)(intptr_t)(handle.handleGPU.ptr);

						ImVec2 mousePos = ImGui::GetMousePos();
						ImGui::SetNextWindowPos(ImVec2(mousePos.x + 16, mousePos.y + 16));
						ImGui::SetNextWindowBgAlpha(0.85f);
						ImGui::Begin("Preview", nullptr,
									 ImGuiWindowFlags_NoTitleBar |
									 ImGuiWindowFlags_NoResize |
									 ImGuiWindowFlags_AlwaysAutoResize |
									 ImGuiWindowFlags_NoSavedSettings |
									 ImGuiWindowFlags_NoFocusOnAppearing |
									 ImGuiWindowFlags_NoNav |
									 ImGuiWindowFlags_NoMove);
						ImGui::Image(texID, ImVec2(128, 128));
						ImGui::End();
					}
				}
			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}


}

void Material::SetMaterialData(ModelMaterialData materialData) {
	material_->color = materialData.color;
	material_->enableLighting = true;
	material_->uvTransform = Matrix4x4::MakeUnit();
	material_->shininess = 50;
	textureName_ = materialData.textureFilePath;
}