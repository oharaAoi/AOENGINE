#pragma once
#include <string>
#include "Engine/DirectX/Pipeline/PipelineGroup/ComputeShaderPipeline.h"

class ComputeShaderPipelines {
public:

	ComputeShaderPipelines() = default;
	~ComputeShaderPipelines() = default;

	void Finalize();

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="dxCompiler">コンパイラー</param>
	/// <param name="shaders">シェーダーファイルパスをまとめたクラス</param>
	void Init(ID3D12Device* device, DirectXCompiler* dxCompiler);

	void Load(const std::string& path);

	json LoadJson(const std::string& directory, const std::string& fileName);

	void SetPipeline(ID3D12GraphicsCommandList* commandList, const std::string& typeName);

	Pipeline* GetLastUsedPipeline() const { return lastUsedPipeline_; }

private:

	const std::string kEngineAssets = "./Engine/Assets/Pipeline/CS/";
	const  std::string kGameAssets = "./Game/Assets/Pipeline/CS/";

	std::unique_ptr<ComputeShaderPipeline> computeShaderPipeline_;

	// inputLayout
	InputLayout inputLayout_;

	// DXCで使う
	DirectXCompiler* dxCompiler_ = nullptr;
	// device
	ID3D12Device* device_ = nullptr;

	Pipeline* lastUsedPipeline_;
};

