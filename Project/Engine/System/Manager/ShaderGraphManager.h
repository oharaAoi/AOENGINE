#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "Engine/System/Asset/AssetHandle.h"

namespace AOENGINE {

class ShaderGraph;

/// ShaderGraph外部ファイルの登録、共有ロード、AssetHandle解決を担当します。
class ShaderGraphManager {
public:
	static ShaderGraphManager* GetInstance();
	/// D3D12デバイスを破棄する前に、ロード済みGraphとGPU参照を解放します。
	void Finalize();

	std::optional<AssetHandle> RegisterAsset(const std::filesystem::path& path);
	std::shared_ptr<ShaderGraph> Load(const std::string& path);
	std::shared_ptr<ShaderGraph> Resolve(const AssetHandle& handle);
	std::string GetPath(const AssetHandle& handle) const;
	bool IsShaderGraphAsset(const std::filesystem::path& path) const;

private:
	struct AssetData {
		AssetHandle handle{ AssetType::ShaderGraph };
		std::string path;
		std::shared_ptr<ShaderGraph> graph;
	};

	static std::string NormalizePath(const std::filesystem::path& path);

	std::unordered_map<std::string, AssetData> assetsByPath_;
	std::unordered_map<uint32_t, std::string> pathById_;
};

}
