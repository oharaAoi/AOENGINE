#pragma once

#include <string>
#include <nlohmann/json_fwd.hpp>

namespace AOENGINE {

class SceneRenderer;
class Canvas2d;
class PostProcess;

class SceneSerializer {
public:
	static nlohmann::json Serialize(const std::string& sceneName, const SceneRenderer& renderer,
		const PostProcess& postProcess);
	static bool Deserialize(const nlohmann::json& root, SceneRenderer& renderer, Canvas2d& canvas,
		PostProcess& postProcess);

	static bool Save(const std::string& folderPath, const std::string& fileName,
		const SceneRenderer& renderer, const PostProcess& postProcess);
	static bool Load(const std::string& folderPath, const std::string& fileName,
		SceneRenderer& renderer, Canvas2d& canvas, PostProcess& postProcess);
};

}
