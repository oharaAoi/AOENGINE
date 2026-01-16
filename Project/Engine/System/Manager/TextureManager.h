#pragma once
#include <d3d12.h>
#include <cassert>
#include <vector>
#include <memory>
#include <map>
#include <stack>
#include <Externals/DirectXTex/DirectXTex.h>
#include <Externals/DirectXTex/d3dx12.h>
#include "Engine/Utilities/Convert.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/DirectX/Resource/DxResourceManager.h"
#include "Engine/Lib/Math/Vector2.h"

namespace AOENGINE {

class TextureManager {
public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static TextureManager* GetInstance();

public:

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	const TextureManager& operator=(const TextureManager&) = delete;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device"></param>
	void Init(ID3D12Device* _dxDevice, ID3D12GraphicsCommandList* _commandList, AOENGINE::DescriptorHeap* _dxHeap, AOENGINE::DxResourceManager* _resourceManger);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// Stackに入っているTextureをすべて読み込む
	/// </summary>
	void LoadStack();

	static void LoadTexture(const std::string& directoryPath, const std::string& filePath);

	void LoadTextureFile(const std::string& directoryPath, const std::string& filePath);

	/// <summary>
	/// Textrueデータを読む
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	DirectX::ScratchImage LoadMipImage(const std::string& directoryPath, const std::string& filePath);

	/// <summary>
	/// TextureResourceにデータを転送する
	/// </summary>
	/// <param name="texture"></param>
	/// <param name="mipImage"></param>
	/// <param name="device"></param>
	/// <param name="commandList"></param>
	/// <returns></returns>
	[[nodiscard]]
	ComPtr<ID3D12Resource> UploadTextureData(
		Microsoft::WRL::ComPtr<ID3D12Resource> texture,
		const DirectX::ScratchImage& mipImage,
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

	/// <summary>
	/// ResourceDescを作成する
	/// </summary>
	/// <param name="metadata"></param>
	/// <returns></returns>
	D3D12_RESOURCE_DESC CreateResourceDesc(const DirectX::TexMetadata& metadata);

	/// <summary>
	/// Textureを選択し名前を返す
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	std::string SelectTexture(const std::string& filePath);

	/// <summary>
	/// textureのpreviewを出す
	/// </summary>
	/// <returns></returns>
	bool PreviewTexture(std::string& _textureName);

public:


	/// <summary>
	/// textureをコマンドに積む
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="textureNum"></param>
	void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, const std::string& filePath, const uint32_t& rootParameterIndex);

	AOENGINE::DxResource* GetResource(const std::string& _textureName) { return textureData_.at(_textureName).resource_; }

	uint32_t GetSRVDataIndex() { return static_cast<uint32_t>(textureData_.size()); }

	const Math::Vector2 GetTextureSize(const std::string& filePath);

	const std::vector<std::string>& GetFileNames() const { return fileNames_; }

	const DescriptorHandles& GetDxHeapHandles(const std::string& fileName) const { return textureData_.at(fileName).resource_->GetSRV(); }

	void StackTexture(const std::string& directoryPath, const std::string& filePath);

private:

	struct TextureData {
		AOENGINE::DxResource* resource_;
		ComPtr<ID3D12Resource> intermediateResource_ = nullptr;
		Math::Vector2 textureSize_;
	};

	struct TexturePath {
		std::string directory;
		std::string fileName;
	};

	std::vector<std::string> fileNames_;

	// TextureDataのコンテナ
	std::unordered_map<std::string, TextureData> textureData_;

	// 読み込み予定のTexture
	std::stack<TexturePath> loadStack_;

	// 生成で使う変数
	ID3D12Device* device_;
	AOENGINE::DescriptorHeap* dxHeap_;
	ID3D12GraphicsCommandList* commandList_;
	AOENGINE::DxResourceManager* resourceManager_;
};

}