#include "GpuParticles.h"
#include "Engine/Engine.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Utilities/Loader.h"
#include <System/Manager/MeshManager.h>
#include <System/Manager/ModelManager.h>

GpuParticles::GpuParticles() {}
GpuParticles::~GpuParticles() {}

void GpuParticles::Finalize() {
	perViewBuffer_.Reset();
	perFrameBuffer_.Reset();
	particleResource_->Destroy();
	freeListIndexResource_->Destroy();
	freeListResource_->Destroy();
	meshArray_.clear();
	materials_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticles::Init(uint32_t instanceNum) {
	// ポインタの取得
	AOENGINE::GraphicsContext* graphicsCxt = AOENGINE::GraphicsContext::GetInstance();
	ID3D12GraphicsCommandList* commandList = graphicsCxt->GetCommandList();

	kInstanceNum_ = instanceNum;

	// gpuに送るResourceの作成
	particleResource_ = graphicsCxt->CreateDxResource(ResourceType::Common);
	freeListIndexResource_ = graphicsCxt->CreateDxResource(ResourceType::Common);
	freeListResource_ = graphicsCxt->CreateDxResource(ResourceType::Common);

	// propertiesの作成
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_HEAP_PROPERTIES defaultHeapProperties{};
	defaultHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// descの作成
	D3D12_RESOURCE_DESC resourceDesc = CreateUavResourceDesc(sizeof(Particle) * kInstanceNum_);
	D3D12_RESOURCE_DESC freeListIndexDesc = CreateUavResourceDesc(sizeof(uint32_t) * kInstanceNum_);
	D3D12_RESOURCE_DESC freeListDesc = CreateUavResourceDesc(sizeof(uint32_t) * kInstanceNum_);

	particleResource_->CreateResource(&resourceDesc, &defaultHeapProperties,
									  D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);

	freeListIndexResource_->CreateResource(&freeListIndexDesc, &defaultHeapProperties,
										   D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON); 
	freeListResource_->CreateResource(&freeListDesc, &defaultHeapProperties,
									  D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);

	// 各UAV, SRV
	particleResource_->CreateUAV(CreateUavDesc(kInstanceNum_, sizeof(Particle)));
	particleResource_->CreateSRV(CreateSrvDesc(kInstanceNum_, sizeof(Particle)));

	freeListIndexResource_->CreateUAV(CreateUavDesc(kInstanceNum_, sizeof(uint32_t)));
	freeListIndexResource_->CreateSRV(CreateSrvDesc(kInstanceNum_, sizeof(uint32_t)));

	freeListResource_->CreateUAV(CreateUavDesc(kInstanceNum_, sizeof(uint32_t)));
	freeListResource_->CreateSRV(CreateSrvDesc(kInstanceNum_, sizeof(uint32_t)));


	perViewBuffer_ = CreateBufferResource(AOENGINE::GraphicsContext::GetInstance()->GetDevice(), sizeof(PerView));
	perViewBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&perView_));

	// ゲーム情報
	perView_->viewProjection = Math::Matrix4x4::MakeUnit();
	perView_->billboardMat = Math::Matrix4x4::MakeUnit();

	perFrameBuffer_ = CreateBufferResource(AOENGINE::GraphicsContext::GetInstance()->GetDevice(), sizeof(PerFrame));
	perFrameBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&perFrame_));

	// mesh・Material情報o
	meshArray_ = AOENGINE::MeshManager::GetInstance()->GetMeshes("sphere.obj");
	materialData_ = LoadMaterialData(AOENGINE::ModelManager::GetModelPath("sphere.obj"), "sphere.obj");

	for (const auto& material : materialData_) {
		auto& newMaterial = materials_.emplace_back(std::make_unique<AOENGINE::Material>());
		newMaterial->Init();
		newMaterial->SetMaterialData(material.second);
	}

	// -------------------------------------------------
	// ↓ Particleの初期化をGPUで行う
	// -------------------------------------------------

	//Engine::SetCsPipeline(CsPipelineType::GpuParticleInit);
	InitBindCmdList(commandList, 0);
	commandList->Dispatch((UINT)kInstanceNum_ / 1024, 1, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticles::Update() {
	perFrame_->deltaTime = AOENGINE::GameTimer::DeltaTime();
	perFrame_->time = AOENGINE::GameTimer::TotalTime();

	ID3D12GraphicsCommandList* commandList = AOENGINE::GraphicsContext::GetInstance()->GetCommandList();
	//Engine::SetCsPipeline(CsPipelineType::GpuParticleUpdate);
	commandList->SetComputeRootDescriptorTable(0, particleResource_->GetUAV().handleGPU);
	commandList->SetComputeRootDescriptorTable(1, freeListIndexResource_->GetUAV().handleGPU);
	commandList->SetComputeRootDescriptorTable(2, freeListResource_->GetUAV().handleGPU);
	commandList->SetComputeRootConstantBufferView(3, perFrameBuffer_->GetGPUVirtualAddress());
	commandList->Dispatch((UINT)kInstanceNum_ / 1024, 1, 1);

	// UAVの変更
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.UAV.pResource = particleResource_->GetResource();
	commandList->ResourceBarrier(1, &barrier);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticles::Draw(ID3D12GraphicsCommandList* commandList){
	for (uint32_t oi = 0; oi < meshArray_.size(); oi++) {
		commandList->IASetVertexBuffers(0, 1, &meshArray_[oi]->GetVBV());
		commandList->IASetIndexBuffer(&meshArray_[oi]->GetIBV());
		commandList->SetGraphicsRootConstantBufferView(0, materials_[oi]->GetBufferAddress());
		commandList->SetGraphicsRootDescriptorTable(1, particleResource_->GetSRV().handleGPU);
		commandList->SetGraphicsRootConstantBufferView(3, perViewBuffer_->GetGPUVirtualAddress());

		std::string textureName = materials_[oi]->GetAlbedoTexture();
		AOENGINE::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureName, 2);

		commandList->DrawIndexedInstanced(meshArray_[oi]->GetIndexNum(), kInstanceNum_, 0, 0, 0);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticles::InitBindCmdList(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex) {
	commandList->SetComputeRootDescriptorTable(rootParameterIndex, particleResource_->GetUAV().handleGPU);
	commandList->SetComputeRootDescriptorTable(rootParameterIndex + 1, freeListIndexResource_->GetUAV().handleGPU);
	commandList->SetComputeRootDescriptorTable(rootParameterIndex + 2, freeListResource_->GetUAV().handleGPU);
}

void GpuParticles::EmitBindCmdList(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex) {
	commandList->SetComputeRootDescriptorTable(rootParameterIndex, particleResource_->GetUAV().handleGPU);
	commandList->SetComputeRootDescriptorTable(rootParameterIndex + 1, freeListIndexResource_->GetUAV().handleGPU);
	commandList->SetComputeRootDescriptorTable(rootParameterIndex + 2, freeListResource_->GetUAV().handleGPU);
}

void GpuParticles::SetViewProjection(const Math::Matrix4x4& viewProjection) {
	perView_->viewProjection = viewProjection;
}
