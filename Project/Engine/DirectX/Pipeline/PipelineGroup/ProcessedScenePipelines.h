#pragma once
#include "Engine/DirectX/Pipeline/PipelineGroup/IPipelineGroup.h"

enum class ProcessedScenePSO {
	Normal,
	Grayscale,
	RadialBlur,
	GlitchNoise,
	Vignette,
};

/// <summary>
/// Scene全体の描画が終わった後に行うような処理のPipelineをまとめたクラス
/// </summary>
class ProcessedScenePipelines :
	public IPipelineGroup {
public:

	ProcessedScenePipelines() = default;
	~ProcessedScenePipelines() override;

	void Init(ID3D12Device* device, DirectXCompiler* dxCompiler) override;

	void SetPipeline(ID3D12GraphicsCommandList* commandList, const std::string& typeName);

	void AddPipeline(const std::string& fileName, json jsonData);

private:

	

};

