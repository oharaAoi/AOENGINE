#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <assert.h>
#include "Engine/Core/Engine.h"
#include "Engine/Module/Components/GameObject/Model.h"

namespace AOENGINE {

/// <summary>
/// Modelを管理するクラス
/// </summary>
class ModelManager {
public: // constructor

	ModelManager() = default;
	~ModelManager();
	ModelManager(const ModelManager&) = delete;
	const ModelManager& operator=(const ModelManager&) = delete;

	static ModelManager* GetInstance();

public: // public method

	// 初期化
	void Init();

	// 終了
	void Finalize();

	/// <summary>
	/// modelを読み込む
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="modelName"></param>
	void LoadModel(const std::string& directoryPath, const std::string& modelName);

public: // public method

	/// <summary>
	/// modelを取得する
	/// </summary>
	/// <param name="modelName"></param>
	/// <returns></returns>
	AOENGINE::Model* GetModel(const std::string& modelName);

	/// <summary>
	/// modelのパスを取得
	/// </summary>
	/// <param name="modelName"></param>
	/// <returns></returns>
	std::string GetModelPath(const std::string& modelName);

	std::vector<std::string>& GetModelNameList() { return modelNameList_; }
	std::vector<std::string>& GetModelNames() { return GetInstance()->GetModelNameList(); }

private: // private variable

	std::unordered_map<std::string, std::unique_ptr<AOENGINE::Model>> modelMap_;
	std::vector<std::string> modelNameList_;

	std::unordered_map<std::string, std::string> modelPathMap_;
};

}