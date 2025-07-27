#include "ComputeShaderPipeline.h"

ComputeShaderPipeline::~ComputeShaderPipeline() {
	pipelineMap_.clear();
}

void ComputeShaderPipeline::Init(ID3D12Device* device, DirectXCompiler* dxCompiler) {
	assert(device);
	assert(dxCompiler);

	device_ = device;
	dxCompiler_ = dxCompiler;
}

void ComputeShaderPipeline::SetPipeline(ID3D12GraphicsCommandList* commandList, const std::string& typeName) {
	pipelineMap_[typeName]->SetComputeState(commandList);
	lastUsePipeline_ = pipelineMap_[typeName].get();
}

void ComputeShaderPipeline::AddPipeline(const std::string& fileName, json jsonData) {
	pipelineMap_[fileName] = std::make_unique<Pipeline>();
	pipelineMap_[fileName]->Init(device_, dxCompiler_, jsonData);
}
