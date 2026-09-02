#pragma once

#include <string>
#include <nlohmann/json_fwd.hpp>

#include "Engine/Module/Components/GameObject/SceneObject.h"

namespace AOENGINE {

class Canvas2d;
class SceneRenderer;

/// <summary>
/// 指定したSceneObjectとその子階層をPrefabとして保存・生成する。
/// Prefab内のIDはファイル内だけで有効で、生成されたSceneObjectには新しいScene IDが割り当てられる。
/// </summary>
class PrefabSerializer {
public:
	static nlohmann::json Serialize(
		const std::string& prefabName, ObjectHandle rootHandle, const SceneRenderer& renderer);

	/// <summary>既存シーンを消さずにPrefabを追加し、生成したルートを返す。</summary>
	static SceneObject* Deserialize(
		const nlohmann::json& root, SceneRenderer& renderer, Canvas2d& canvas);

	/// <summary>folderPath/prefabName.prefab.jsonへ保存する。</summary>
	static bool Save(const std::string& folderPath, const std::string& prefabName,
		ObjectHandle rootHandle, const SceneRenderer& renderer);

	/// <summary>folderPath/prefabName.prefab.jsonから生成する。</summary>
	static SceneObject* Load(const std::string& folderPath, const std::string& prefabName,
		SceneRenderer& renderer, Canvas2d& canvas);
};

}
