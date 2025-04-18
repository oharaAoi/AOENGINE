#include "SpritePipelines.h"

SpritePipelines::~SpritePipelines() {
	pipelineMap_.clear();
}

void SpritePipelines::Init(ID3D12Device* device, DirectXCompiler* dxCompiler, Shader* shaders) {
	assert(device);
	assert(dxCompiler);

	pipelineMap_[SpritePSO::None] = std::make_unique<Pipeline>();
	pipelineMap_[SpritePSO::Normal] = std::make_unique<Pipeline>();
	pipelineMap_[SpritePSO::Add] = std::make_unique<Pipeline>();
	pipelineMap_[SpritePSO::Subtract] = std::make_unique<Pipeline>();
	pipelineMap_[SpritePSO::Multily] = std::make_unique<Pipeline>();
	pipelineMap_[SpritePSO::Screen] = std::make_unique<Pipeline>();

	pipelineMap_[SpritePSO::None]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Sprite), RootSignatureType::Sprite, inputLayout_.CreateSprite(), Blend::ModeNone, true, true);
	pipelineMap_[SpritePSO::Normal]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Sprite), RootSignatureType::Sprite, inputLayout_.CreateSprite(), Blend::ModeNormal, true, true);
	pipelineMap_[SpritePSO::Add]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Sprite), RootSignatureType::Sprite, inputLayout_.CreateSprite(), Blend::ModeAdd, true, true);
	pipelineMap_[SpritePSO::Subtract]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Sprite), RootSignatureType::Sprite, inputLayout_.CreateSprite(), Blend::ModeSubtract, true, true);
	pipelineMap_[SpritePSO::Multily]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Sprite), RootSignatureType::Sprite, inputLayout_.CreateSprite(), Blend::ModeMultily, true, true);
	pipelineMap_[SpritePSO::Multily]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Sprite), RootSignatureType::Sprite, inputLayout_.CreateSprite(), Blend::ModeScreen, true, true);
}

void SpritePipelines::SetPipeline(ID3D12GraphicsCommandList* commandList, SpritePSO kind) {
	pipelineMap_[kind]->Draw(commandList);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
