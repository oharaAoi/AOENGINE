#pragma once

#include <string>

namespace AOENGINE {

class SceneRenderer;
class Canvas2d;

class SceneSerializer {
public:
	static bool Save(const std::string& folderPath, const std::string& fileName,
		const SceneRenderer& renderer);
	static bool Load(const std::string& folderPath, const std::string& fileName,
		SceneRenderer& renderer, Canvas2d& canvas);
};

}
