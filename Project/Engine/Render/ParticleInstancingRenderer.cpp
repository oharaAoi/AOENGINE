#include "ParticleInstancingRenderer.h"
#include "Engine.h"

ParticleInstancingRenderer::~ParticleInstancingRenderer() {
	for (auto& particle : particleMap_) {
		particle.second.particleResource_.Reset();
		DescriptorHeap::AddFreeSrvList(particle.second.srvHandle_.assignIndex_);
	}
	perViewBuffer_.Reset();
}

void ParticleInstancingRenderer::Init(uint32_t instanceNum) {
	maxInstanceNum_ = instanceNum;

	perViewBuffer_ = CreateBufferResource(Engine::GetDevice(), sizeof(PerView));
	perViewBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&perView_));
}

void ParticleInstancingRenderer::Update(const std::string& id, const std::vector<ParticleData>& particleData) {
	for (uint32_t oi = particleMap_[id].useIndex; oi < particleData.size(); ++oi) {
		particleMap_[id].particleData[oi].color = { 0,0,0,0 };
	}
	
	for (uint32_t oi = particleMap_[id].useIndex; oi < particleData.size(); ++oi) {
		if (oi < maxInstanceNum_) {
			particleMap_[id].particleData[oi].worldMat = particleData[oi].worldMat;
			particleMap_[id].particleData[oi].color = particleData[oi].color;

			particleMap_[id].useIndex = oi;
		}
	}
}

void ParticleInstancingRenderer::PostUpdate() {
	for (auto& information : particleMap_) {
		information.second.useIndex = 0;
	}
}

void ParticleInstancingRenderer::Draw(ID3D12GraphicsCommandList* commandList) const {
	for (auto& information : particleMap_) {
		commandList->IASetVertexBuffers(0, 1, &information.second.pMesh->GetVBV());
		commandList->IASetIndexBuffer(&information.second.pMesh->GetIBV());
		commandList->SetGraphicsRootConstantBufferView(0, information.second.materials->GetBufferAdress());
		commandList->SetGraphicsRootDescriptorTable(1, information.second.srvHandle_.handleGPU);
		std::string textureName = information.second.materials->GetUseTexture();
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureName, 2);
		commandList->SetGraphicsRootConstantBufferView(3, perViewBuffer_->GetGPUVirtualAddress());

		commandList->DrawIndexedInstanced(information.second.pMesh->GetIndexNum(), maxInstanceNum_, 0, 0, 0);
	}
}

void ParticleInstancingRenderer::AddParticle(const std::string& id, Mesh* _pMesh, Material* _pMaterial) {
	auto it = particleMap_.find(id);
	if (it != particleMap_.end()) {
		return;		// 見つかったら早期リターン
	}
	
	ID3D12Device* device = Engine::GetDevice();
	DescriptorHeap* dxHeap = Engine::GetDxHeap();

	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_HEAP_PROPERTIES defaultHeapProperties{};
	defaultHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// -----------------------------------------------------------------
	Information particles;
	particles.pMesh = _pMesh; 
	particles.materials = _pMaterial;
	
	particles.particleResource_ = CreateBufferResource(device, sizeof(ParticleData) * maxInstanceNum_);
	particles.particleData = nullptr;
	particles.particleResource_->Map(0, nullptr, reinterpret_cast<void**>(&particles.particleData));
	particles.srvHandle_ = dxHeap->AllocateSRV();

	D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	desc.Buffer.NumElements = maxInstanceNum_;
	desc.Buffer.StructureByteStride = sizeof(ParticleData);
	device->CreateShaderResourceView(particles.particleResource_.Get(), &desc, particles.srvHandle_.handleCPU);

	for(uint32_t index =0; index < maxInstanceNum_; ++index){
		particles.particleData->worldMat = Matrix4x4::MakeUnit();
		particles.particleData->color = { 1,1,1,1 };
	}

	particleMap_.emplace(id, std::move(particles));
}
