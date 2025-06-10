#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Engine/Lib/Math/MathStructures.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class SceneLoader {
public:		// data

	/// <summary>
	/// ファイルに記載されている各objectの情報
	/// </summary>
	struct Objects {
		std::string meshName;
		std::string modelName;
		QuaternionSRT srt;
	};

	/// <summary>
	/// 各objectをまとめたデータ
	/// </summary>
	struct LevelData {
		std::vector<Objects> objects;
	};

public:

	SceneLoader() = default;
	~SceneLoader() = default;
	SceneLoader(const SceneLoader&) = delete;
	const SceneLoader& operator=(const SceneLoader&) = delete;

	static SceneLoader* GetInstance();

	void Init();

	void Load(const std::string& directory, const std::string& fileName, const std::string& extension);

public:

	Objects GetObjects(const std::string& objName) const;

private:

	std::unique_ptr<LevelData> levelData_;

};

