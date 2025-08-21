#include "ParticleInstancingRenderer.h"
#include "Engine.h"

ParticleInstancingRenderer::~ParticleInstancingRenderer() {
	for (auto& particle : particleMap_) {
		particle.second.particleResource_.Reset();
		DescriptorHeap::AddFreeSrvList(particle.second.srvHandle_.assignIndex_);
	}
	particleMap_.clear();
	perViewBuffer_.Reset();
}

void ParticleInstancingRenderer::Init(uint32_t instanceNum) {
	maxInstanceNum_ = instanceNum;

	perViewBuffer_ = CreateBufferResource(GraphicsContext::GetInstance()->GetDevice(), sizeof(PerView));
	perViewBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&perView_));
}

void ParticleInstancingRenderer::Update(const std::string& id, const std::vector<ParticleData>& particleData, bool addBlend) {
	uint32_t currentUseIndex = particleMap_[id].useIndex;
	
	// 現在使用しているindexから引数のサイズ分colorを0にする
	for (uint32_t oi = 0; oi < particleData.size(); ++oi) {
		particleMap_[id].particleData[currentUseIndex + oi].color = { 0,0,0,0 };
	}

	// 現在使用しているindexからデータを更新する
	//uint32_t maxParticlesCount = currentUseIndex + static_cast<uint32_t>(particleData.size());
	for (uint32_t oi = 0; oi < particleData.size(); ++oi) {
		if (currentUseIndex + oi < maxInstanceNum_) {
			particleMap_[id].particleData[currentUseIndex + oi].worldMat = particleData[oi].worldMat;
			particleMap_[id].particleData[currentUseIndex + oi].color = particleData[oi].color;
			// 使用しているindexを更新する
			particleMap_[id].useIndex = oi + 1;
		}
	}

	particleMap_[id].isAddBlend = addBlend;
}

void ParticleInstancingRenderer::PostUpdate() {
	// 使用しなかったindexの領域のcolorを0にする
	for (auto& information : particleMap_) {
		// 使用している分を取得
		uint32_t startIndex = information.second.useIndex;
		// 使用分からmaxまでを0にする
		for (uint32_t index = startIndex; index < maxInstanceNum_; index++) {
			information.second.particleData[index].color = { 0,0,0,0 };
		}
	}

	// 次のフレームで最初から更新するためにすべてのindexを0にする
	for (auto& information : particleMap_) {
		information.second.useIndex = 0;
	}
}

void ParticleInstancingRenderer::Draw(ID3D12GraphicsCommandList* commandList) const {
	for (auto& information : particleMap_) {
		if (information.second.isAddBlend) {
			Engine::SetPipeline(PSOType::Object3d, "Object_Particle.json");
		} else {
			Engine::SetPipeline(PSOType::Object3d, "Object_ParticleSubtract.json");
		}

		Pipeline* pso = Engine::GetLastUsedPipeline();

		commandList->IASetVertexBuffers(0, 1, &information.second.pMesh->GetVBV());
		commandList->IASetIndexBuffer(&information.second.pMesh->GetIBV());

		UINT index = pso->GetRootSignatureIndex("gMaterial");
		commandList->SetGraphicsRootConstantBufferView(index, information.second.materials->GetBufferAddress());
		index = pso->GetRootSignatureIndex("gParticles");
		commandList->SetGraphicsRootDescriptorTable(index, information.second.srvHandle_.handleGPU);

		index = pso->GetRootSignatureIndex("gTexture");
		std::string textureName = information.second.materials->GetAlbedoTexture();
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureName, index);
		index = pso->GetRootSignatureIndex("gPerView");
		commandList->SetGraphicsRootConstantBufferView(index, perViewBuffer_->GetGPUVirtualAddress());

		commandList->DrawIndexedInstanced(information.second.pMesh->GetIndexNum(), maxInstanceNum_, 0, 0, 0);
	}
}

std::shared_ptr<Material> ParticleInstancingRenderer::AddParticle(const std::string& id, const std::string& textureName, std::shared_ptr<Mesh> _pMesh, bool isAddBlend) {
	auto it = particleMap_.find(id);
	if (it != particleMap_.end()) {
		return particleMap_[id].materials;		// 見つかったら早期リターン
	}
	GraphicsContext* graphicsCtx = GraphicsContext::GetInstance();

	ID3D12Device* device = graphicsCtx->GetDevice();
	DescriptorHeap* dxHeap = graphicsCtx->GetDxHeap();

	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_HEAP_PROPERTIES defaultHeapProperties{};
	defaultHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// -----------------------------------------------------------------
	Information particles;
	particles.pMesh = _pMesh;
	particles.materials = std::make_shared<Material>();
	particles.materials->Init();
	particles.textureName = textureName;
	
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

	for (uint32_t index = 0; index < maxInstanceNum_; ++index) {
		particles.particleData->worldMat = Matrix4x4::MakeUnit();
		particles.particleData->color = { 0,0,0,0 };
	}

	particles.isAddBlend = isAddBlend;

	particleMap_.emplace(id, std::move(particles));
	
	return particleMap_[id].materials;
}

void ParticleInstancingRenderer::ChangeMesh(const std::string& id, std::shared_ptr<Mesh> _mesh) {
	auto it = particleMap_.find(id);
	if (it == particleMap_.end()) {
		assert(false && "対応する名前が見つかりませんでした");
	}
	
	particleMap_[id].pMesh = _mesh;
}
