#pragma once

#include <nlohmann/json_fwd.hpp>

namespace AOENGINE {

class Canvas2d;
class SceneObject;
class SceneRenderer;

/// <summary>SceneとPrefabで共有するSceneObject単体の変換処理。</summary>
class SceneObjectSerializer {
public:
	static nlohmann::json Serialize(const SceneObject& object);
	static SceneObject* Deserialize(
		const nlohmann::json& objectJson, SceneRenderer& renderer, Canvas2d& canvas);
};

}
