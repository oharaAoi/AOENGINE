#pragma once
#include <memory>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "Engine/Lib/Math/MathStructures.h"

using json = nlohmann::json;

/// <summary>
/// level配置されたファイルを読み込むクラス
/// </summary>
class LevelLoader {
public:		// data

	/// <summary>
	/// ファイルに記載されている各objectの情報
	/// </summary>
	struct Objects {
		std::string modelName;
		QuaternionSRT srt;
	};

	/// <summary>
	/// 各objectをまとめたデータ
	/// </summary>
	struct LevelData {
		std::vector<Objects> objects;
	};

public:		// base

	LevelLoader() = default;
	~LevelLoader() = default;

	void Load(const std::string& directory, const std::string& fileName, const std::string& extension);

private:	// variable
	
	std::unique_ptr<LevelData> levelData_;

};

