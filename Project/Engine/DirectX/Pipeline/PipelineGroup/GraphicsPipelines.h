#pragma once
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

	GraphicsPipelines();
	~GraphicsPipelines();

	void Finalize();

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="dxCompiler">コンパイラー</param>
	/// <param name="shaders">シェーダーファイルパスをまとめたクラス</param>
	void Init(ID3D12Device* device, DirectXCompiler* dxCompiler, Shader* shaders);

	void SetPipeline(ID3D12GraphicsCommandList* commandList, Object3dPSO kind);
	void SetPipeline(ID3D12GraphicsCommandList* commandList, SpritePSO kind);
	void SetPipeline(ID3D12GraphicsCommandList* commandList, ProcessedScenePSO kind);

private:

	std::unique_ptr<Object3dPipelines> obj3dPipeline_;
	std::unique_ptr<SpritePipelines> spritePipeline_;
	std::unique_ptr<ProcessedScenePipelines> processedScenePipeline_;

	// inputLayout
	InputLayout inputLayout_;

	// DXCで使う
	DirectXCompiler* dxCompiler_ = nullptr;
	// device
	ID3D12Device* device_ = nullptr;

};

