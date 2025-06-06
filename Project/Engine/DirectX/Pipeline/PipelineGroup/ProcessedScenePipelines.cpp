#include "ProcessedScenePipelines.h"

ProcessedScenePipelines::~ProcessedScenePipelines() {
	pipelineMap_.clear();
}

void ProcessedScenePipelines::Init(ID3D12Device* device, DirectXCompiler* dxCompiler) {
	assert(device);
	assert(dxCompiler);

	device_ = device;
	dxCompiler_ = dxCompiler;
}

void ProcessedScenePipelines::SetPipeline(ID3D12GraphicsCommandList* commandList, const std::string& typeName) {
	pipelineMap_[typeName]->Draw(commandList);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	lastUsePipeline_ = pipelineMap_[typeName].get();
}

void ProcessedScenePipelines::AddPipeline(const std::string& fileName, json jsonData) {
	pipelineMap_[fileName] = std::make_unique<Pipeline>();
	pipelineMap_[fileName]->Init(device_, dxCompiler_, jsonData);
}
