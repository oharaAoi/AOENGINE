#pragma once
#include <list>
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
		std::string name;
		std::string modelName;
		QuaternionSRT srt;

		std::string colliderType = "";
		Vector3 colliderCenter;
		Vector3 colliderSize;
		float colliderRadius;
		
		std::list<Objects> children;
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

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// Scene読み込み処理
	/// </summary>
	/// <param name="directory"></param>
	/// <param name="fileName"></param>
	/// <param name="extension"></param>
	void Load(const std::string& directory, const std::string& fileName, const std::string& extension);

	/// <summary>
	/// objectの読み込み処理
	/// </summary>
	/// <param name="objectJson"></param>
	/// <returns></returns>
	Objects LoadObject(const json& objectJson);

public:

	Objects GetObjects(const std::string& objName) const;

	const LevelData* GetLevelData() const { return levelData_.get(); }

private:

	std::unique_ptr<LevelData> levelData_;

};

