#include "Object3dPipelines.h"

Object3dPipelines::~Object3dPipelines() {
	pipelineMap_.clear();
}

void Object3dPipelines::Init(ID3D12Device* device, DirectXCompiler* dxCompiler, Shader* shaders) {
	assert(device);
	assert(dxCompiler);

	pipelineMap_[Object3dPSO::Normal] = std::make_unique<Pipeline>();
	pipelineMap_[Object3dPSO::NormalCut] = std::make_unique<Pipeline>();
	pipelineMap_[Object3dPSO::NormalEnviroment] = std::make_unique<Pipeline>();
	pipelineMap_[Object3dPSO::Add] = std::make_unique<Pipeline>();
	pipelineMap_[Object3dPSO::TextureLess] = std::make_unique<Pipeline>();
	pipelineMap_[Object3dPSO::Particle] = std::make_unique<Pipeline>();
	pipelineMap_[Object3dPSO::SubParticle] = std::make_unique<Pipeline>();
	pipelineMap_[Object3dPSO::Skybox] = std::make_unique<Pipeline>();
	pipelineMap_[Object3dPSO::TextureBlendNone] = std::make_unique<Pipeline>();
	pipelineMap_[Object3dPSO::TextureBlendNormal] = std::make_unique<Pipeline>();
	pipelineMap_[Object3dPSO::TextureBlendAdd] = std::make_unique<Pipeline>();

	pipelineMap_[Object3dPSO::Normal]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Normal), RootSignatureType::Normal, inputLayout_.Create(), Blend::ModeNormal, true, true, true);
	pipelineMap_[Object3dPSO::NormalCut]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Normal), RootSignatureType::Normal, inputLayout_.Create(), Blend::ModeNormal, false, true, true);
	pipelineMap_[Object3dPSO::NormalEnviroment]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::NormalEnviroment), RootSignatureType::NormalEnviroment, inputLayout_.Create(), Blend::ModeNormal, true, true, true);
	pipelineMap_[Object3dPSO::Add]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Normal), RootSignatureType::Normal, inputLayout_.Create(), Blend::ModeAdd, true, true, true);
	pipelineMap_[Object3dPSO::TextureLess]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::TextureLess), RootSignatureType::TextureLess, inputLayout_.Create(), Blend::ModeNormal, true, true, true);
	pipelineMap_[Object3dPSO::Particle]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Particle), RootSignatureType::Particle, inputLayout_.CreateParticle(), Blend::ModeAdd,true, false, true);
	pipelineMap_[Object3dPSO::SubParticle]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Particle), RootSignatureType::Particle, inputLayout_.CreateParticle(), Blend::ModeNormal,true, false, true);
	pipelineMap_[Object3dPSO::Skybox]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::Skybox), RootSignatureType::Skybox, inputLayout_.Create(), Blend::ModeNormal, true, false, true);
	pipelineMap_[Object3dPSO::TextureBlendNone]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::ObjectTextureBlend), RootSignatureType::TextureBlend, inputLayout_.Create(), Blend::ModeNone, true, false, true);
	pipelineMap_[Object3dPSO::TextureBlendNormal]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::ObjectTextureBlend), RootSignatureType::TextureBlend, inputLayout_.Create(), Blend::ModeNormal, true, false, true);
	pipelineMap_[Object3dPSO::TextureBlendAdd]->Initialize(device, dxCompiler, shaders->GetShaderData(Shader::ObjectTextureBlend), RootSignatureType::TextureBlend, inputLayout_.Create(), Blend::ModeAdd, true, false, true);

}

void Object3dPipelines::SetPipeline(ID3D12GraphicsCommandList* commandList, Object3dPSO kind) {
	pipelineMap_[kind]->Draw(commandList);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}