#include "Object3dPipelines.h"

Object3dPipelines::~Object3dPipelines() {
	pipelineMap_.clear();
}

void Object3dPipelines::Init(ID3D12Device* device, DirectXCompiler* dxCompiler, Shader* shaders) {
	assert(device);
	assert(dxCompiler);

	pipelineMap_[Object3dPSO::Normal] = std::make_unique<Pipeline>();
	pipelineMap_[Object3dPSO::Add] = std::make_unique<Pipeline>();
	pipelineMap_[Object3dPSO::TextureLess] = std::make_unique<Pipeline>(); 
	pipelineMap_[Object3dPSO::Particle] = std::make_unique<Pipeline>();
	pipelineMap_[Object3dPSO::TextureLess] = std::make_unique<Pipeline>();

	pipelineMap_[Object3dPSO::Normal]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Normal), RootSignatureType::Normal, inputLayout_.Create(), Blend::ModeNormal, true, true);
	pipelineMap_[Object3dPSO::Add]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Normal), RootSignatureType::Normal, inputLayout_.Create(), Blend::ModeAdd, true, true);
	pipelineMap_[Object3dPSO::TextureLess]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::TextureLess), RootSignatureType::TextureLess, inputLayout_.Create(), Blend::ModeNormal, true, true);
	pipelineMap_[Object3dPSO::Particle]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Particle), RootSignatureType::Particle, inputLayout_.CreateParticle(), Blend::ModeAdd, true, true);
	pipelineMap_[Object3dPSO::TextureLess]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::PBR), RootSignatureType::PBR, inputLayout_.Create(), Blend::ModeNormal, true, true);
}

void Object3dPipelines::SetPipeline(ID3D12GraphicsCommandList* commandList, Object3dPSO kind) {
	pipelineMap_[kind]->Draw(commandList);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
