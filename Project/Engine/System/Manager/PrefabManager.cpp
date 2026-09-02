#include "PrefabManager.h"

#include "Engine/Core/Engine.h"
#include "Engine/Lib/Json/JsonSerializer.h"
#include "Engine/Lib/Path.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Scene/PrefabSerializer.h"
#include "Engine/Utilities/Logger.h"

#include <fstream>
#include <iomanip>

using namespace AOENGINE;

namespace {

bool IsValidPrefabName(const std::string& name) {
	if (name.empty() || name == "." || name == ".." ||
		name.ends_with(".prefab") || name.ends_with(".prefab.json")) {
		return false;
	}
	return name.find_first_of("<>:\"/\\|?*") == std::string::npos;
}

std::filesystem::path GetSceneDataRoot() {
	return std::filesystem::path(kAssetPath) / "Game/GameData/JsonItems";
}

bool LoadJsonFile(const std::filesystem::path& path, nlohmann::json& result) {
	std::ifstream stream(path);
	if (!stream) { return false; }
	try {
		stream >> result;
		return true;
	} catch (...) {
		return false;
	}
}

bool SaveJsonFile(const std::filesystem::path& path, const nlohmann::json& value) {
	std::ofstream stream(path, std::ios::trunc);
	if (!stream) { return false; }
	stream << std::setw(4) << value << std::endl;
	return stream.good();
}

size_t ReplaceScenePrefabReferences(nlohmann::json& scene, const std::string& oldName,
	const std::string& newName) {
	if (!scene.is_object() || scene.value("format", "") != "AOENGINE_SCENE" ||
		!scene.contains("objects") || !scene.at("objects").is_array()) {
		return 0;
	}

	size_t count = 0;
	for (nlohmann::json& object : scene["objects"]) {
		if (object.value("type", "") != "PrefabInstance" || !object.contains("data")) { continue; }
		nlohmann::json& data = object["data"];
		if (data.value("prefab", "") == oldName) {
			data["prefab"] = newName;
			++count;
		}
	}
	return count;
}

}

PrefabManager::PrefabManager()
	: prefabDirectory_(kAssetPath + "/Game/Prefabs/") {}

PrefabManager* PrefabManager::GetInstance() {
	static PrefabManager instance;
	return &instance;
}

bool PrefabManager::LoadPrefab(const std::string& prefabName, bool forceReload) {
	if (prefabName.empty()) { return false; }
	if (!forceReload && prefabCache_.contains(prefabName)) { return true; }

	nlohmann::json prefab = JsonSerializer::Load(prefabDirectory_, prefabName + ".prefab");
	if (!prefab.is_object() || prefab.value("format", "") != "AOENGINE_PREFAB" ||
		prefab.value("version", 0) != 1) {
		Logger::Log("[Prefab][Load] Not found or invalid: " + prefabDirectory_ + prefabName + ".prefab.json\n");
		return false;
	}

	prefabCache_[prefabName] = std::move(prefab);
	return true;
}

SceneObject* PrefabManager::Instantiate(const std::string& prefabName) {
	SceneRenderer* renderer = SceneRenderer::GetInstance();
	Canvas2d* canvas = Engine::GetCanvas2d();
	if (!renderer || !canvas) { return nullptr; }
	return Instantiate(prefabName, *renderer, *canvas);
}

SceneObject* PrefabManager::Instantiate(
	const std::string& prefabName, SceneRenderer& renderer, Canvas2d& canvas) {
	if (!LoadPrefab(prefabName)) { return nullptr; }
	SceneObject* root = PrefabSerializer::Deserialize(prefabCache_.at(prefabName), renderer, canvas);
	if (!root) {
		Logger::Log("[Prefab][Instantiate] Failed: " + prefabName + "\n");
	} else {
		// キャッシュ内の表示名ではなく、検索に使用したアセット名を参照として保持する。
		root->SetPrefabSource(prefabName);
	}
	return root;
}

bool PrefabManager::SavePrefab(const std::string& prefabName, ObjectHandle rootHandle) {
	SceneRenderer* renderer = SceneRenderer::GetInstance();
	return renderer && SavePrefab(prefabName, rootHandle, *renderer);
}

bool PrefabManager::SavePrefab(const std::string& prefabName, ObjectHandle rootHandle,
	SceneRenderer& renderer) {
	if (prefabName.empty()) { return false; }
	nlohmann::json prefab = PrefabSerializer::Serialize(prefabName, rootHandle, renderer);
	if (prefab.is_null() ||
		!JsonSerializer::Save(prefabDirectory_, prefabName + ".prefab", prefab)) {
		Logger::Log("[Prefab][Save] Failed: " + prefabDirectory_ + prefabName + ".prefab.json\n");
		return false;
	}

	prefabCache_[prefabName] = std::move(prefab);
	if (SceneObject* root = renderer.FindObject(rootHandle)) {
		root->SetPrefabSource(prefabName);
	}
	return true;
}

PrefabOperationResult PrefabManager::RenamePrefab(const std::string& oldName, const std::string& newName) {
	if (!IsValidPrefabName(oldName) || !IsValidPrefabName(newName)) { return PrefabOperationResult::InvalidName; }
	if (oldName == newName) { return PrefabOperationResult::Success; }

	const std::filesystem::path oldPath = std::filesystem::path(prefabDirectory_) / (oldName + ".prefab.json");
	const std::filesystem::path newPath = std::filesystem::path(prefabDirectory_) / (newName + ".prefab.json");
	std::error_code error;
	if (!std::filesystem::exists(oldPath, error) || error) { return PrefabOperationResult::NotFound; }
	if (std::filesystem::exists(newPath, error) && !error) { return PrefabOperationResult::AlreadyExists; }

	nlohmann::json prefab;
	if (!LoadJsonFile(oldPath, prefab) || prefab.value("format", "") != "AOENGINE_PREFAB") {
		return PrefabOperationResult::InvalidPrefab;
	}
	prefab["prefabName"] = newName;
	if (!SaveJsonFile(newPath, prefab)) { return PrefabOperationResult::FileSystemError; }

	const std::filesystem::path sceneRoot = GetSceneDataRoot();
	if (std::filesystem::exists(sceneRoot, error) && !error) {
		for (const auto& entry : std::filesystem::recursive_directory_iterator(sceneRoot)) {
			if (!entry.is_regular_file() || entry.path().extension() != ".json") { continue; }
			nlohmann::json scene;
			if (!LoadJsonFile(entry.path(), scene)) { continue; }
			if (ReplaceScenePrefabReferences(scene, oldName, newName) != 0 && !SaveJsonFile(entry.path(), scene)) {
				std::filesystem::remove(newPath, error);
				return PrefabOperationResult::FileSystemError;
			}
		}
	}

	SceneRenderer* renderer = SceneRenderer::GetInstance();
	if (renderer) {
		for (const ObjectHandle& handle : renderer->GetObjectHandles()) {
			SceneObject* object = renderer->FindObject(handle);
			if (object && object->GetPrefabSource() == oldName) { object->SetPrefabSource(newName); }
		}
	}
	prefabCache_.erase(oldName);
	prefabCache_[newName] = std::move(prefab);
	if (!std::filesystem::remove(oldPath, error) || error) { return PrefabOperationResult::FileSystemError; }
	Logger::Log("[Prefab][Rename] " + oldName + " -> " + newName + "\n");
	return PrefabOperationResult::Success;
}

PrefabReferenceInfo PrefabManager::FindReferences(const std::string& prefabName) const {
	PrefabReferenceInfo info;
	if (const SceneRenderer* renderer = SceneRenderer::GetInstance()) {
		for (const ObjectHandle& handle : renderer->GetObjectHandles()) {
			const SceneObject* object = renderer->FindObject(handle);
			if (object && object->GetPrefabSource() == prefabName) { ++info.activeInstanceCount; }
		}
	}

	std::error_code error;
	const std::filesystem::path sceneRoot = GetSceneDataRoot();
	if (!std::filesystem::exists(sceneRoot, error) || error) { return info; }
	for (const auto& entry : std::filesystem::recursive_directory_iterator(sceneRoot)) {
		if (!entry.is_regular_file() || entry.path().extension() != ".json") { continue; }
		nlohmann::json scene;
		if (!LoadJsonFile(entry.path(), scene)) { continue; }
		if (ReplaceScenePrefabReferences(scene, prefabName, prefabName) != 0) {
			info.sceneFiles.push_back(entry.path().string());
		}
	}
	return info;
}

PrefabOperationResult PrefabManager::DeletePrefab(const std::string& prefabName, bool forceDelete) {
	if (!IsValidPrefabName(prefabName)) { return PrefabOperationResult::InvalidName; }
	const std::filesystem::path path = std::filesystem::path(prefabDirectory_) / (prefabName + ".prefab.json");
	std::error_code error;
	if (!std::filesystem::exists(path, error) || error) { return PrefabOperationResult::NotFound; }
	if (!forceDelete && FindReferences(prefabName).HasReferences()) {
		return PrefabOperationResult::ReferencedByScene;
	}
	if (!std::filesystem::remove(path, error) || error) { return PrefabOperationResult::FileSystemError; }

	prefabCache_.erase(prefabName);
	if (SceneRenderer* renderer = SceneRenderer::GetInstance()) {
		for (const ObjectHandle& handle : renderer->GetObjectHandles()) {
			SceneObject* object = renderer->FindObject(handle);
			if (object && object->GetPrefabSource() == prefabName) { object->ClearPrefabSource(); }
		}
	}
	Logger::Log("[Prefab][Delete] " + prefabName + "\n");
	return PrefabOperationResult::Success;
}

void PrefabManager::UnloadPrefab(const std::string& prefabName) {
	prefabCache_.erase(prefabName);
}

void PrefabManager::Clear() {
	prefabCache_.clear();
}

bool PrefabManager::IsLoaded(const std::string& prefabName) const {
	return prefabCache_.contains(prefabName);
}

void PrefabManager::SetPrefabDirectory(const std::string& directory) {
	if (directory.empty() || directory == prefabDirectory_) { return; }
	prefabDirectory_ = directory;
	if (prefabDirectory_.back() != '/' && prefabDirectory_.back() != '\\') {
		prefabDirectory_ += '/';
	}
	Clear();
}
