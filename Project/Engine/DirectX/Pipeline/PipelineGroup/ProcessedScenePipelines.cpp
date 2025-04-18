#include "ProcessedScenePipelines.h"

ProcessedScenePipelines::~ProcessedScenePipelines() {
	pipelineMap_.clear();
}

void ProcessedScenePipelines::Init(ID3D12Device* device, DirectXCompiler* dxCompiler, Shader* shaders) {
	assert(device);
	assert(dxCompiler);

	pipelineMap_[ProcessedScenePSO::Normal] = std::make_unique<Pipeline>();
	pipelineMap_[ProcessedScenePSO::Grayscale] = std::make_unique<Pipeline>();

	pipelineMap_[ProcessedScenePSO::Normal]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::ProcessedScene), RootSignatureType::ProcessedScene, inputLayout_.CreateRenderTexture(), Blend::ModeNone, true, false);
	pipelineMap_[ProcessedScenePSO::Grayscale]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Grayscale), RootSignatureType::ProcessedScene, inputLayout_.CreateRenderTexture(), Blend::ModeNone, true, false);
}

void ProcessedScenePipelines::SetPipeline(ID3D12GraphicsCommandList* commandList, ProcessedScenePSO kind) {
	pipelineMap_[kind]->Draw(commandList);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
