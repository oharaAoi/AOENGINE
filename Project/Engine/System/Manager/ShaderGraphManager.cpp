#include "ShaderGraphManager.h"

#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

#include "Engine/System/ShaderGraph/ShaderGraph.h"

using namespace AOENGINE;

ShaderGraphManager* ShaderGraphManager::GetInstance() {
	static ShaderGraphManager instance;
	return &instance;
}

void ShaderGraphManager::Finalize() {
	// Graphの各NodeはDxResourceや定数バッファを保持するため、
	// GraphicsContext/D3D12Deviceより先に破棄する必要があります。
	assetsByPath_.clear();
	pathById_.clear();
}

std::string ShaderGraphManager::NormalizePath(const std::filesystem::path& path) {
	std::error_code ec;
	auto normalized = std::filesystem::weakly_canonical(path, ec);
	if (ec) { normalized = path.lexically_normal(); }
	std::string key = normalized.generic_string();
	std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
	});
	return key;
}

bool ShaderGraphManager::IsShaderGraphAsset(const std::filesystem::path& path) const {
	std::string name = path.filename().string();
	std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
	});
	if (name.ends_with(".shadergraph.json") || name.ends_with(".aosg")) { return true; }

	if (!name.ends_with(".json")) { return false; }
	std::ifstream stream(path);
	if (!stream) { return false; }
	try {
		const nlohmann::json root = nlohmann::json::parse(stream);
		return root.value("assetType", "") == "ShaderGraph";
	} catch (...) {
		return false;
	}
}

std::optional<AssetHandle> ShaderGraphManager::RegisterAsset(const std::filesystem::path& path) {
	if (!IsShaderGraphAsset(path)) { return std::nullopt; }
	const std::string key = NormalizePath(path);
	auto found = assetsByPath_.find(key);
	if (found != assetsByPath_.end()) { return found->second.handle; }

	AssetData data;
	data.path = path.lexically_normal().generic_string();
	const AssetHandle handle = data.handle;
	pathById_[handle.id] = key;
	assetsByPath_.emplace(key, std::move(data));
	return handle;
}

std::shared_ptr<ShaderGraph> ShaderGraphManager::Load(const std::string& path) {
	const auto handle = RegisterAsset(path);
	if (!handle) { return nullptr; }
	const std::string key = pathById_.at(handle->id);
	AssetData& data = assetsByPath_.at(key);
	if (!data.graph) {
		data.graph = std::make_shared<ShaderGraph>();
		data.graph->Init(std::filesystem::path(data.path).stem().string());
		data.graph->Load(data.path);
	}
	return data.graph;
}

std::shared_ptr<ShaderGraph> ShaderGraphManager::Resolve(const AssetHandle& handle) {
	if (handle.type != AssetType::ShaderGraph) { return nullptr; }
	const std::string path = GetPath(handle);
	return path.empty() ? nullptr : Load(path);
}

std::string ShaderGraphManager::GetPath(const AssetHandle& handle) const {
	const auto id = pathById_.find(handle.id);
	if (id == pathById_.end()) { return {}; }
	const auto asset = assetsByPath_.find(id->second);
	return asset != assetsByPath_.end() ? asset->second.path : std::string{};
}
