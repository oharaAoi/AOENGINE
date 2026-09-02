#include "PrefabManager.h"

#include "Engine/Core/Engine.h"
#include "Engine/Lib/Json/JsonSerializer.h"
#include "Engine/Lib/Path.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Scene/PrefabSerializer.h"
#include "Engine/Utilities/Logger.h"

using namespace AOENGINE;

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
