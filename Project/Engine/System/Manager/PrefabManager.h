#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>

#include "Engine/Module/Components/GameObject/SceneObject.h"

namespace AOENGINE {

class Canvas2d;
class SceneRenderer;

enum class PrefabOperationResult {
	Success,
	NotFound,
	AlreadyExists,
	InvalidName,
	ReferencedByScene,
	FileSystemError,
	InvalidPrefab,
};

struct PrefabReferenceInfo {
	std::vector<std::string> sceneFiles;
	size_t activeInstanceCount = 0;
	bool HasReferences() const { return activeInstanceCount != 0 || !sceneFiles.empty(); }
};

/// <summary>Prefabアセットの読み込み、キャッシュ、名前による生成を管理する。</summary>
class PrefabManager {
public:
	PrefabManager(const PrefabManager&) = delete;
	PrefabManager& operator=(const PrefabManager&) = delete;

	static PrefabManager* GetInstance();

	/// <summary>Prefab名をキャッシュへ読み込む。forceReloadでディスクから再読込する。</summary>
	bool LoadPrefab(const std::string& prefabName, bool forceReload = false);

	/// <summary>Prefab名から現在のSceneRendererへ生成する。</summary>
	SceneObject* Instantiate(const std::string& prefabName);

	/// <summary>Prefab名から指定したSceneRendererへ生成する。</summary>
	SceneObject* Instantiate(
		const std::string& prefabName, SceneRenderer& renderer, Canvas2d& canvas);

	/// <summary>指定オブジェクト階層をPrefabとして保存し、キャッシュも更新する。</summary>
	bool SavePrefab(const std::string& prefabName, ObjectHandle rootHandle);
	bool SavePrefab(const std::string& prefabName, ObjectHandle rootHandle,
		SceneRenderer& renderer);

	PrefabOperationResult RenamePrefab(const std::string& oldName, const std::string& newName);
	PrefabOperationResult DeletePrefab(const std::string& prefabName, bool forceDelete = false);
	PrefabReferenceInfo FindReferences(const std::string& prefabName) const;

	void UnloadPrefab(const std::string& prefabName);
	void Clear();
	bool IsLoaded(const std::string& prefabName) const;

	void SetPrefabDirectory(const std::string& directory);
	const std::string& GetPrefabDirectory() const { return prefabDirectory_; }

private:
	PrefabManager();

	std::string prefabDirectory_;
	std::unordered_map<std::string, nlohmann::json> prefabCache_;
};

}
