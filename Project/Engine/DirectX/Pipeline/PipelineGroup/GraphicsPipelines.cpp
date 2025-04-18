#include "GraphicsPipelines.h"

GraphicsPipelines::GraphicsPipelines() {
}

GraphicsPipelines::~GraphicsPipelines() {
}

void GraphicsPipelines::Finalize() {
	obj3dPipeline_.reset();
	spritePipeline_.reset();
	processedScenePipeline_.reset();
}

void GraphicsPipelines::Init(ID3D12Device* device, DirectXCompiler* dxCompiler, Shader* shaders) {
	assert(device);
	assert(dxCompiler);
	
	device_ = device;
	dxCompiler_ = dxCompiler;
	
	obj3dPipeline_ = std::make_unique<Object3dPipelines>();
	spritePipeline_ = std::make_unique<SpritePipelines>();
	processedScenePipeline_ = std::make_unique<ProcessedScenePipelines>();

	obj3dPipeline_->Init(device, dxCompiler, shaders);
	spritePipeline_->Init(device, dxCompiler, shaders);
	processedScenePipeline_->Init(device, dxCompiler, shaders);
}

void GraphicsPipelines::SetPipeline(ID3D12GraphicsCommandList* commandList, Object3dPSO kind) {
	obj3dPipeline_->SetPipeline(commandList, kind);
}

void GraphicsPipelines::SetPipeline(ID3D12GraphicsCommandList* commandList, SpritePSO kind) {
	spritePipeline_->SetPipeline(commandList, kind);
}

void GraphicsPipelines::SetPipeline(ID3D12GraphicsCommandList* commandList, ProcessedScenePSO kind) {
	processedScenePipeline_->SetPipeline(commandList, kind);
}
