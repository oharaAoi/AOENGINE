#include "TextureManager.h"
#include "Engine/Utilities/Logger.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Utilities/Loader.h"

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

void TextureManager::Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DescriptorHeap* dxHeap) {
	assert(device);
	assert(dxHeap);

	device_ = device;
	dxHeap_ = dxHeap;

	textureData_.clear();

	commandList_ = commandList;
}

void TextureManager::Finalize() {
	for (auto& data : textureData_) {
		data.second.textureResource_.Reset();
		data.second.intermediateResource_.Reset();
		DescriptorHeap::AddFreeSrvList(data.second.address_.assignIndex_);
	}
}

void TextureManager::LoadStack() {
	Logger::CommentLog("Loading Textures");
	while (!loadStack_.empty()) {
		auto texturePath = loadStack_.top();
		LoadTextureFile(texturePath.directory, texturePath.fileName);
		loadStack_.pop();
	}
}

void TextureManager::LoadTexture(const std::string& directoryPath, const std::string& filePath) {
	GetInstance()->LoadTextureFile(directoryPath, filePath);
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

	Logger::Log("[Load][Texture] :" + filePath);
	TextureData data{};

	DirectX::ScratchImage mipImage = LoadMipImage(directoryPath, filePath);
	const DirectX::TexMetadata& metadata = mipImage.GetMetadata();

	// resourceDescの作成
	D3D12_RESOURCE_DESC desc = CreateResourceDesc(metadata);

	// HEAPの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// shaderResourceの作成
	data.textureResource_ = CerateShaderResource(device_, &desc, &heapProperties, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST);
	data.intermediateResource_ = UploadTextureData(data.textureResource_, mipImage, device_, commandList_);

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
	data.address_ = dxHeap_->AllocateSRV();
	data.textureSize_.x = static_cast<float>(metadata.width);
	data.textureSize_.y = static_cast<float>(metadata.height);
	
	// 配列に入れる
	textureData_[filePath] = data;

	// 生成
	device_->CreateShaderResourceView(data.textureResource_.Get(), &srvDesc, data.address_.handleCPU);

	Logger::Log(" --- success!\n");
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
//	White1x1のTextureを読み込む
/////////////////////////////////////////////////////////////////////////////////////////////
void TextureManager::LoadWhite1x1Texture(const std::string& directoryPath, const std::string& filePath, ID3D12GraphicsCommandList* commandList) {
	TextureData data{};

	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertWString(directoryPath + filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	const DirectX::TexMetadata& metadata = image.GetMetadata();

	// resourceDescの作成
	D3D12_RESOURCE_DESC desc = CreateResourceDesc(metadata);

	// HEAPの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// shaderResourceの作成
	data.textureResource_ = CerateShaderResource(device_, &desc, &heapProperties, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST);
	data.intermediateResource_ = UploadTextureData(data.textureResource_, image, device_, commandList);

	// ------------------------------------------------------------
	// metadataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// ------------------------------------------------------------
	// SRVを作成するDescriptorHeapの場所を求める
	data.address_ = dxHeap_->AllocateSRV();
	data.textureSize_.x = static_cast<float>(metadata.width);
	data.textureSize_.y = static_cast<float>(metadata.height);

	// mapに値を代入
	textureData_.emplace(filePath, data);

	// 生成
	device_->CreateShaderResourceView(data.textureResource_.Get(), &srvDesc, data.address_.handleCPU);
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
		const D3D12_SUBRESOURCE_DATA& sub = subresources[i];
		assert(sub.pData != nullptr);  // 転送元ポインタが有効か
		assert(sub.RowPitch > 0);
		assert(sub.SlicePitch >= sub.RowPitch);
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

const Vector2 TextureManager::GetTextureSize(const std::string& filePath) {
	return  textureData_[filePath].textureSize_;
}

void TextureManager::StackTexture(const std::string& directoryPath, const std::string& filePath) {
	loadStack_.push(TexturePath{directoryPath, filePath });
}

void TextureManager::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, const std::string& filePath, const uint32_t& rootParameterIndex) {
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, textureData_[filePath].address_.handleGPU);
}

std::string TextureManager::SelectTexture(const std::string& filePath) {
	static std::string selectedFilename;
	static int selectedIndex = -1; // 選択されたインデックス

	ImGui::Separator();
	ImGui::BulletText("TextureView");

	// -------------------------------------------------
	// ↓ 現在のTextureのViewを表示
	// -------------------------------------------------
	auto currentHandle = this->GetDxHeapHandles(filePath);
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
			ImGui::TreePop();
			return selectedFilename.c_str();
		}
		// ListBox を手動で構築（ListBoxHeader + Selectable）
		if (ImGui::BeginListBox("TextureList")) {
			for (int i = 0; i < fileNames_.size(); ++i) {
				std::string textureName = fileNames_[i];
				const char* ext = GetFileExtension(textureName.c_str());
				std::string extension(ext);
				// 拡張子で判別する
				if ((extension == "png") || (extension == "jpeg")) {

					const bool isSelected = (i == selectedIndex);
					if (ImGui::Selectable(textureName.c_str(), isSelected)) {
						selectedIndex = i;
						selectedFilename = fileNames_[i];
					}
					// ホバー中のファイルにプレビューを表示
					if (ImGui::IsItemHovered()) {
						auto handle = this->GetDxHeapHandles(textureName);
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

	return filePath;
}