#include "TextureManager.h"
#include <algorithm>
#include "Engine/Utilities/Logger.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Utilities/Loader.h"
#include "Engine/Utilities/Convert.h"
#include "Engine/Utilities/ConvertDDS.h"

using namespace AOENGINE;

TextureManager* AOENGINE::TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// 終了処理
/////////////////////////////////////////////////////////////////////////////////////////////

void TextureManager::Finalize() {
	for (auto& data : textureData_) {
		data.second.resource_->Destroy();
		data.second.intermediateResource_.Reset();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// 初期化処理
/////////////////////////////////////////////////////////////////////////////////////////////

void TextureManager::Init(ID3D12Device* _dxDevice, ID3D12GraphicsCommandList* _commandList, AOENGINE::DescriptorHeap* _dxHeap, AOENGINE::DxResourceManager* _resourceManager) {
	assert(_dxDevice);
	assert(_dxHeap);

	device_ = _dxDevice;
	dxHeap_ = _dxHeap;

	textureData_.clear();
	 
	commandList_ = _commandList;
	resourceManager_ = _resourceManager;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// 読み込み処理
/////////////////////////////////////////////////////////////////////////////////////////////

void TextureManager::LoadStack() {
	// 画像ファイルをddsに変換する
#ifdef _DEBUG
	AOENGINE::Logger::CommentLog("Conversion Image To DDS");
	ConvertAllTexturesFromStack(loadStack_, L"./Project/Packages/Converter/convert.ps1", "./Project/Packages/Converter/ConvertedDDS/");
#endif
	// ddsファイルを読み込む
	LoadFileDDS("./Project/Packages/Converter/ConvertedDDS/");
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Textureの読み込み予約
/////////////////////////////////////////////////////////////////////////////////////////////

void TextureManager::StackTexture(const std::string& directoryPath, const std::string& filePath) {
	loadStack_.push(TexturePath{ directoryPath, filePath });
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Textureを読み込む
/////////////////////////////////////////////////////////////////////////////////////////////

void TextureManager::LoadTextureFile(const std::string& directoryPath, const std::string& filePath) {
	// 一度読み込んだファイルか確認する
	auto it = textureData_.find(filePath);
	if (it != textureData_.end()) {
		return;
	}

	// 配列に格納しておく
	fileNames_.push_back(filePath);

	AOENGINE::Logger::Log("[Load][Texture] :" + filePath);
	TextureData data{};

	DirectX::ScratchImage mipImage = LoadMipImage(directoryPath, filePath);
	const DirectX::TexMetadata& metadata = mipImage.GetMetadata();

	// resourceDescの作成
	D3D12_RESOURCE_DESC desc = CreateResourceDesc(metadata);

	// HEAPの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// shaderResourceの作成
	data.resource_ = resourceManager_->CreateResource(ResourceType::Common);
	data.resource_->CreateResource(&desc, &heapProperties, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST);
	data.intermediateResource_ = UploadTextureData(data.resource_->GetCompResource(), mipImage, device_, commandList_);

	// ------------------------------------------------------------
	// metadataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (metadata.IsCubemap()) {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	} else {
		//srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;  // リソースと同じに
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	}
	
	// ------------------------------------------------------------
	// SRVを作成するDescriptorHeapの場所を求める
	data.resource_->CreateSRV(srvDesc);
	data.textureSize_.x = static_cast<float>(metadata.width);
	data.textureSize_.y = static_cast<float>(metadata.height);
	
	// 配列に入れる
	// 生成
	device_->CreateShaderResourceView(data.resource_->GetCompResource().Get(), &srvDesc, data.resource_->GetSRV().handleCPU);
	std::filesystem::path path = filePath;
	textureData_[path.stem().string()] = std::move(data);
	AOENGINE::Logger::Log(" --- success!\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Textrueデータを読む
/////////////////////////////////////////////////////////////////////////////////////////////

DirectX::ScratchImage TextureManager::LoadMipImage(const std::string& directoryPath, const std::string& filePath) {
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertWString(directoryPath + filePath);
	HRESULT hr;
	// より安全な方法で行うべき
	if (filePathW.ends_with(L".dds")) {
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	} else {
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	assert(SUCCEEDED(hr));

	// ミニマップの作成
	DirectX::ScratchImage mipImages{};
	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		mipImages = std::move(image);
	} else {
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 4, mipImages);
	}
	
	assert(SUCCEEDED(hr));

	return mipImages;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// TextureResourceにデータを転送する
/////////////////////////////////////////////////////////////////////////////////////////////

[[nodiscard]]
ComPtr<ID3D12Resource> TextureManager::UploadTextureData(ComPtr<ID3D12Resource> texture,
														const DirectX::ScratchImage& mipImage,
														 ComPtr<ID3D12Device> device,
														 ComPtr<ID3D12GraphicsCommandList> commandList) {
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device.Get(), mipImage.GetImages(), mipImage.GetImageCount(), mipImage.GetMetadata(), subresources); // subresourceの生成
	UINT64 intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));						// 必要なサイズを求める
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);

	assert(commandList);  // コマンドリストが有効か
	assert(texture);      // 書き込み先リソースが有効か
	assert(intermediateResource); // 中間バッファが有効か
	assert(!subresources.empty()); // サブリソースが空でないか
	assert(subresources.data() != nullptr); // ポインタが有効か

	for (size_t i = 0; i < subresources.size(); ++i) {
		assert(subresources[i].pData != nullptr);  // 転送元ポインタが有効か
		assert(subresources[i].RowPitch > 0);
		assert(subresources[i].SlicePitch >= subresources[i].RowPitch);
	}

	// データ転送をコマンドに積む
	UpdateSubresources(commandList.Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// ResourceDescを作成する
/////////////////////////////////////////////////////////////////////////////////////////////

D3D12_RESOURCE_DESC TextureManager::CreateResourceDesc(const DirectX::TexMetadata& metadata) {
	// metaDataを元にResourceを設定
	D3D12_RESOURCE_DESC desc{};
	desc.Format = metadata.format;									// TextureのFormat
	desc.Width = UINT(metadata.width);								// Textureの幅
	desc.Height = UINT(metadata.height);							// Textureの高さ
	desc.MipLevels = UINT16(metadata.mipLevels);					// mipmapの数
	desc.DepthOrArraySize = UINT16(metadata.arraySize);				// 奥行き　or 配列Textureの配数
	desc.SampleDesc.Count = 1;										// サンプリングカウント
	desc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);	// Textureの次元数

	return desc;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Textureのサイズを返す
/////////////////////////////////////////////////////////////////////////////////////////////

const Math::Vector2 TextureManager::GetTextureSize(const std::string& filePath) {
	std::filesystem::path path = filePath;
	std::string name = path.stem().string();
	auto it = textureData_.find(name);
	if (it == textureData_.end()) {
		std::string comment = name + "が見つかりません(TextureManager::GetTextureSize)";
		AOENGINE::Logger::AssertLog(comment);
	}
	return textureData_[name].textureSize_;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// TextureのDirectXHandleを返す
/////////////////////////////////////////////////////////////////////////////////////////////

const DescriptorHandles& AOENGINE::TextureManager::GetDxHeapHandles(const std::string& fileName) const {
	std::filesystem::path path = fileName;
	std::string name = path.stem().string();
	auto it = textureData_.find(name);
	if (it == textureData_.end()) {
		std::string comment = name + "が見つかりません(TextureManager::GetDxHeapHandles)";
		AOENGINE::Logger::AssertLog(comment);
	}
	return textureData_.at(name).resource_->GetSRV();
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Textureのリソースを返す
/////////////////////////////////////////////////////////////////////////////////////////////

AOENGINE::DxResource* AOENGINE::TextureManager::GetResource(const std::string& _textureName) {
	std::filesystem::path path = _textureName;
	std::string name = path.stem().string();
	auto it = textureData_.find(name);
	if (it == textureData_.end()) {
		std::string comment = name + "が見つかりません(TextureManager::GetResource)";
		AOENGINE::Logger::AssertLog(comment);
	}
	return textureData_[name].resource_;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// TextureをCommandListに登録する
/////////////////////////////////////////////////////////////////////////////////////////////

void TextureManager::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, const std::string& filePath, const uint32_t& rootParameterIndex) {
	std::filesystem::path path = filePath;
	std::string name = path.stem().string();
	auto it = textureData_.find(name);
	if (it != textureData_.end()) {
		commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, textureData_.at(name).resource_->GetSRV().handleGPU);
	} else {
		commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, textureData_.at("error").resource_->GetSRV().handleGPU);
		AOENGINE::Logger::Log(filePath + "が読み込まれていません");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Textureを選択する
/////////////////////////////////////////////////////////////////////////////////////////////

std::string TextureManager::SelectTexture(const std::string& filePath) {
	static std::string selectedFilename;
	static int selectedIndex = -1;
	static std::array<char, 128> searchBuffer = {};

	ImGui::Separator();
	ImGui::BulletText("TextureView");

	// 現在のテクスチャプレビュー
	auto currentHandle = this->GetDxHeapHandles(filePath);
	ImTextureID currentTexID = (ImTextureID)(intptr_t)(currentHandle.handleGPU.ptr);
	ImGui::Image(currentTexID, ImVec2(64, 64));

	if (ImGui::TreeNode("Files")) {

		// 検索入力欄を追加
		ImGui::InputTextWithHint("##Search", "Search texture...", searchBuffer.data(), searchBuffer.size());

		ImGui::Text("Selected: %s", selectedFilename.c_str());
		ImGui::SameLine();
		if (ImGui::Button("OK")) {
			ImGui::TreePop();
			return selectedFilename;
		}

		// リスト表示
		if (ImGui::BeginListBox("TextureList")) {
			for (int i = 0; i < fileNames_.size(); ++i) {
				const std::string& textureName = fileNames_[i];
				const char* ext = GetFileExtension(textureName.c_str());
				std::string extension(ext);

				// 拡張子でフィルタ
				if ((extension != "png") && (extension != "jpeg") && (extension != "dds")) {
					continue;
				}

				// 検索文字列でフィルタ
				if (searchBuffer[0] != '\0') {
					std::string lowerName = textureName;
					std::string lowerSearch = searchBuffer.data();
					std::transform(
						lowerName.begin(), lowerName.end(),
						lowerName.begin(),
						[](unsigned char c) { return static_cast<char>(std::tolower(c)); }
					);

					std::transform(
						lowerSearch.begin(), lowerSearch.end(),
						lowerSearch.begin(),
						[](unsigned char c) { return static_cast<char>(std::tolower(c)); }
					);

					if (lowerName.find(lowerSearch) == std::string::npos)
						continue; // 一致しない場合はスキップ
				}

				// 選択リスト
				const bool isSelected = (i == selectedIndex);
				if (ImGui::Selectable(textureName.c_str(), isSelected)) {
					selectedIndex = i;
					selectedFilename = fileNames_[i];
				}

				// ホバー時プレビュー
				if (ImGui::IsItemHovered()) {
					auto handle = this->GetDxHeapHandles(textureName);
					ImTextureID texID = (ImTextureID)(intptr_t)(handle.handleGPU.ptr);

					ImVec2 mousePos = ImGui::GetMousePos();
					ImGui::SetNextWindowPos(ImVec2(mousePos.x + 16, mousePos.y + 16));
					ImGui::SetNextWindowBgAlpha(0.85f);
					ImGui::Begin("Preview", nullptr,
								 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
								 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
								 ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove);
					ImGui::Image(texID, ImVec2(128, 128));
					ImGui::End();
				}
			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}

	return filePath;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Textureのpreviewを返す
/////////////////////////////////////////////////////////////////////////////////////////////

bool TextureManager::PreviewTexture(std::string& _textureName) {
	uint32_t count = 0;
	for (int i = 0; i < fileNames_.size(); ++i) {
		const std::string& textureName = fileNames_[i];
		const char* ext = GetFileExtension(textureName.c_str());
		std::string extension(ext);

		// 拡張子でフィルタ
		if ((extension != "png") && (extension != "jpeg")) {
			continue;
		}

		// textureを表示
		auto handle = this->GetDxHeapHandles(textureName);
		ImTextureID texID = (ImTextureID)(intptr_t)(handle.handleGPU.ptr);
		if (ImGui::ImageButton(textureName.c_str(), texID, ImVec2(64, 64))) {
			_textureName = textureName;	// 書き換える
			return true;
		}
		if (count < 3) {
			ImGui::SameLine();
			count++;
		} else {
			count = 0;
		}
	}
	ImGui::Text("end");
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Stackに溜まっているパスをDDSに変換する
/////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::TextureManager::ConvertAllTexturesFromStack(std::stack<TexturePath>& stack, const std::wstring& scriptPath, const std::string& outputDDSFolder) {
	std::vector<std::wstring> paths;

	while (!stack.empty()) {
		TexturePath tp = stack.top();
		stack.pop();

		if (tp.fileName.find(' ') != std::string::npos) {
			std::string message = "Textureの名前に空白が含まれているため読み込めません(" + tp.fileName + ")";
			AOENGINE::Logger::AssertLog(message);
		}

		std::string fullPath = tp.directory;
		if (!fullPath.empty() && fullPath.back() != '\\' && fullPath.back() != '/') {
			fullPath += "\\";
		}
		fullPath += tp.fileName;

		std::filesystem::path fileNamePath(fullPath);
		std::string filename = fileNamePath.stem().string();
		auto srcTime = std::filesystem::last_write_time(fullPath);
		auto dstTime = std::filesystem::last_write_time(outputDDSFolder + filename + ".dds");

		if (!std::filesystem::exists(outputDDSFolder + filename + ".dds") || srcTime > dstTime) {
			paths.push_back(ConvertWString(fullPath));
		}
	}

	if (paths.empty())
		return;

	RunPowerShellScript(scriptPath, paths);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// DDSファイルを読み込む
/////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::TextureManager::LoadFileDDS(const std::filesystem::path& folderPath) {
	if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath)) {
		std::cerr << "フォルダが存在しません: " << folderPath << std::endl;
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
		if (entry.is_regular_file()) {
			const std::filesystem::path& filePath = entry.path();
			std::cout << "ファイル: " << filePath << std::endl;
			LoadTextureFile(filePath.parent_path().string() + "/", filePath.filename().string());
		}
	}
}
