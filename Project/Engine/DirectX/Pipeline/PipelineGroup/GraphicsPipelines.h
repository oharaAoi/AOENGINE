#pragma once
#include <string>
#include "Engine/DirectX/Pipeline/PipelineGroup/Object3dPipelines.h"
#include "Engine/DirectX/Pipeline/PipelineGroup/SpritePipelines.h"
#include "Engine/DirectX/Pipeline/PipelineGroup/ProcessedScenePipelines.h"

enum class PSOType{
	Object3d,
	Sprite,
	ProcessedScene,
	Primitive,
};

class GraphicsPipelines {
public:

	struct AssetPaths {
		std::string object;
		std::string sprite;
		std::string postProcess;
	};

public:

	GraphicsPipelines();
	~GraphicsPipelines();

	void Finalize();

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="dxCompiler">コンパイラー</param>
	/// <param name="shaders">シェーダーファイルパスをまとめたクラス</param>
	void Init(ID3D12Device* device, DirectXCompiler* dxCompiler);

	void Load(const std::string& path, PSOType type);

	json LoadJson(const std::string& directory, const std::string& fileName);

	void SetPipeline(ID3D12GraphicsCommandList* commandList, PSOType type, const std::string& typeName);

	Pipeline* GetLastUsedPipeline() const { return lastUsedPipeline_; }

private:

	const AssetPaths kEngineAssets = {
		"./Engine/Assets/Pipeline/Object/",
		"./Engine/Assets/Pipeline/Sprite/",
		"./Engine/Assets/Pipeline/PostProcess/"
	};

	const AssetPaths kGameAssets = {
		"./Game/Assets/Pipeline/Object/",
		"./Game/Assets/Pipeline/Sprite/",
		"./Game/Assets/Pipeline/PostProcess/"
	};

	std::unique_ptr<Object3dPipelines> obj3dPipeline_;
	std::unique_ptr<SpritePipelines> spritePipeline_;
	std::unique_ptr<ProcessedScenePipelines> processedScenePipeline_;

	// inputLayout
	InputLayout inputLayout_;

	// DXCで使う
	DirectXCompiler* dxCompiler_ = nullptr;
	// device
	ID3D12Device* device_ = nullptr;

	Pipeline* lastUsedPipeline_;

};

