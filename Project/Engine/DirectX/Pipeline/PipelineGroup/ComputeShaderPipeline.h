#pragma once
// PSO
#include "Engine/DirectX/Pipeline/PipelineGroup/IPipelineGroup.h"


enum class CsPipelineType {
	HorizontalBlur_Pipeline,
	VerticalBlur_Pipeline,
	GrayScale_Pipeline,
	DepthOfField_Pipeline,
	Skinning_Pipeline,
	Blend_Pipeline,
	Result_Pipeline,

	GpuParticleInit,
	GpuParticleUpdate,
	GpuParticleEnd,
	EmitGpuParticle,
};

class ComputeShaderPipeline :
	public IPipelineGroup {
public:

	ComputeShaderPipeline() = default;
	~ComputeShaderPipeline() override;

	void Init(ID3D12Device* device, DirectXCompiler* dxCompiler) override;

	void SetPipeline(ID3D12GraphicsCommandList* commandList, const std::string& typeName);

	void AddPipeline(const std::string& fileName, json jsonData);

private:

};

