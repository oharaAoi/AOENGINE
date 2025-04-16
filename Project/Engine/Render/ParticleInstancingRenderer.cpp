#include "ParticleInstancingRenderer.h"
#include "Engine.h"

ParticleInstancingRenderer::~ParticleInstancingRenderer() {
	for (size_t index = 0; index < particles_.size(); ++index) {
		DescriptorHeap::AddFreeSrvList(particles_[index].instancingSrvHandle.assignIndex_);
	}
}

void ParticleInstancingRenderer::Init(uint32_t instanceNum) {
	maxInstanceNum_ = instanceNum;
}

void ParticleInstancingRenderer::AddParticle(Mesh* _pMesh, Material* _pMaterial) {
	ID3D12Device* device = Engine::GetDevice();

	Information particles;
	particles.pMesh = _pMesh;
	particles.materials = _pMaterial;
	
	particles.particleBuffer = CreateBufferResource(device, sizeof(Information) * maxInstanceNum_);
	particles.particleData = nullptr;
	particles.particleBuffer->Map(0, nullptr, reinterpret_cast<void**>(&particles.particleData));
	for(uint32_t index =0; index < maxInstanceNum_; ++index){
		particles.particleData->worldMat = Matrix4x4::MakeUnit();
		particles.particleData->color = { 1,1,1,1 };
	}


	D3D12_SHADER_RESOURCE_VIEW_DESC desc{}; 
	desc = CreateSrvDesc(maxInstanceNum_, sizeof(Information));
	particles.instancingSrvHandle = Engine::GetDxHeap()->AllocateSRV();

	particles_.push_back(std::move(particles));
}
