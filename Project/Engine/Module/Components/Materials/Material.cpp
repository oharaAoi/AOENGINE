#include "Material.h"
#include "ImGuiFileDialog.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/Utilities/Loader.h"

Material::Material() {
}

Material::~Material() {
	Finalize();
}

void Material::Finalize() {
	materialBuffer_.Reset();
	material_ = nullptr;
}

void Material::Init(ID3D12Device* device, const Model::ModelMaterialData& materialData) {
	// ---------------------------------------------------------------
	// ↓Materialの設定
	// ---------------------------------------------------------------
	materialBuffer_ = CreateBufferResource(device, sizeof(MaterialData));
	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
	// 色を決める
	SetMaterialData(materialData);

	uvTranslation_ = { 0,0,0 };
	uvScale_ = { 1,1,1 };
	uvRotation_ = { 0,0,0 };
}

void Material::Update(const Matrix4x4& uvTransform) {
	material_->uvTransform = uvTransform;
	material_->uvTransform = SRT(uvScale_, uvRotation_, uvTranslation_).MakeAffine();
}

void Material::Draw(ID3D12GraphicsCommandList* commandList) {
	commandList->SetGraphicsRootConstantBufferView(0, materialBuffer_->GetGPUVirtualAddress());
}

#ifdef _DEBUG
void Material::ImGuiDraw() {
	ImGui::BulletText("Transform");
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

	// Textureを変更する
	SelectTexture();

	// 色を変更する
	ImGui::Separator();
	ImGui::BulletText("Color");
	ImGui::ColorEdit4("color", &material_->color.x);
	ImGui::Combo("Lighting", &material_->enableLighting, "None\0Lambert\0HalfLambert");

	//IGFD::FileDialogConfig config;
	//config.path = "."; // 初期フォルダ
	//if (ImGui::Button("画像を選択")) {
	//	// 拡張子フィルタ：.png のみ
	//	ImGuiFileDialog::Instance()->OpenDialog(
	//		"ChoosePngFile",
	//		"PNG画像を選択",
	//		".png",
	//		config
	//	);
	//}

	//// ダイアログ表示・処理
	//if (ImGuiFileDialog::Instance()->Display("ChoosePngFile")) {
	//	if (ImGuiFileDialog::Instance()->IsOk()) {
	//		std::string selectedFile = ImGuiFileDialog::Instance()->GetFilePathName();
	//		ImGui::Text(selectedFile.c_str());
	//		// selectedFile に .png ファイルのパスが入っている
	//		// ここで画像の読み込み処理などが可能
	//	}
	//	ImGuiFileDialog::Instance()->Close();
	//}

	material_->uvTransform = SRT(uvScale_, uvRotation_, uvTranslation_).MakeAffine();
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
	auto currentHandle = textureManager->GetDxHeapHandles(materialsData_.textureFilePath);
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
			SetUseTexture(selectedFilename.c_str());
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
#endif

void Material::SetUseTexture(const std::string& name) {
	materialsData_.textureFilePath = name;
}

void Material::SetMaterialData(Model::ModelMaterialData materialData) {
	materialsData_ = materialData;
	
	material_->color = materialData.color;
	material_->enableLighting = true;
	material_->uvTransform = Matrix4x4::MakeUnit();
	material_->shininess = 50;
	materialsData_.textureFilePath = materialData.textureFilePath;
}