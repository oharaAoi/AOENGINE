#include "GpuParticleRenderer.h"
#include "Engine/Engine.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/Lib/GameTimer.h"

GpuParticleRenderer::~GpuParticleRenderer() {
	perViewBuffer_.Reset();
	perFrameBuffer_.Reset();
	particleResource_->Finalize();
	freeListIndexResource_->Finalize();
	freeListResource_->Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleRenderer::Init(uint32_t _instanceNum) {
	// ポインタの取得
	GraphicsContext* graphicsCxt = GraphicsContext::GetInstance();
	DescriptorHeap* dxHeap = graphicsCxt->GetDxHeap();
	ID3D12Device* dxDevice = graphicsCxt->GetDevice();
	ID3D12GraphicsCommandList* commandList = graphicsCxt->GetCommandList();

	kInstanceNum_ = _instanceNum;
	
	// GPUへ送るResourceの作成
	CreateResource(dxHeap, dxDevice);

	// meshの作成
	shape_ = std::make_unique<GeometryObject>();
	shape_->Set<PlaneGeometry>();
	shape_->GetMaterial()->SetAlbedoTexture("white.png");

	material_ = std::make_unique<Material>();
	material_->Init();
	material_->SetAlbedoTexture("circle.png");

	// 初期化コマンドの実行
	Engine::SetPipelineCS("GpuParticleInit.json");
	Pipeline* pso =  Engine::GetLastUsedPipelineCS();
	UINT index = 0;
	index = pso->GetRootSignatureIndex("gParticles");
	commandList->SetComputeRootDescriptorTable(index, particleResource_->GetUAV().handleGPU);
	index = pso->GetRootSignatureIndex("gFreeListIndex");
	commandList->SetComputeRootDescriptorTable(index, freeListIndexResource_->GetUAV().handleGPU);
	index = pso->GetRootSignatureIndex("gFreeList");
	commandList->SetComputeRootDescriptorTable(index, freeListResource_->GetUAV().handleGPU);
	index = pso->GetRootSignatureIndex("gMaxParticles");
	commandList->SetComputeRootConstantBufferView(index, maxParticleBuffer_->GetGPUVirtualAddress());
	commandList->Dispatch((UINT)kInstanceNum_ / 1024, 1, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleRenderer::Update() {
	perFrame_->deltaTime = GameTimer::DeltaTime();
	perFrame_->time = GameTimer::TotalTime();

	ID3D12GraphicsCommandList* commandList = GraphicsContext::GetInstance()->GetCommandList();
	Engine::SetPipelineCS("GpuParticleUpdate.json");
	Pipeline* pso = Engine::GetLastUsedPipelineCS();
	UINT index = 0;
	index = pso->GetRootSignatureIndex("gParticles");
	commandList->SetComputeRootDescriptorTable(index, particleResource_->GetUAV().handleGPU);
	index = pso->GetRootSignatureIndex("gFreeListIndex");
	commandList->SetComputeRootDescriptorTable(index, freeListIndexResource_->GetUAV().handleGPU);
	index = pso->GetRootSignatureIndex("gFreeList");
	commandList->SetComputeRootDescriptorTable(index, freeListResource_->GetUAV().handleGPU);
	index = pso->GetRootSignatureIndex("gPerFrame");
	commandList->SetComputeRootConstantBufferView(index, perFrameBuffer_->GetGPUVirtualAddress());
	index = pso->GetRootSignatureIndex("gMaxParticles");
	commandList->SetComputeRootConstantBufferView(index, maxParticleBuffer_->GetGPUVirtualAddress());
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

void GpuParticleRenderer::Draw() const {
	Engine::SetPipeline(PSOType::Object3d, "Object_GpuParticle.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	ID3D12GraphicsCommandList* commandList = GraphicsContext::GetInstance()->GetCommandList();

	commandList->IASetVertexBuffers(0, 1, &shape_->GetMesh()->GetVBV());
	commandList->IASetIndexBuffer(&shape_->GetMesh()->GetIBV());

	UINT index = pso->GetRootSignatureIndex("gMaterial");
	commandList->SetGraphicsRootConstantBufferView(index, material_->GetBufferAddress());
	index = pso->GetRootSignatureIndex("gParticles");
	commandList->SetGraphicsRootDescriptorTable(index, particleResource_->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gTexture");
	std::string textureName = material_->GetAlbedoTexture();
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureName, index);
	index = pso->GetRootSignatureIndex("gPerView");
	commandList->SetGraphicsRootConstantBufferView(index, perViewBuffer_->GetGPUVirtualAddress());

	commandList->DrawIndexedInstanced(shape_->GetMesh()->GetIndexNum(), kInstanceNum_, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Resourceの作成
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleRenderer::CreateResource(DescriptorHeap* _dxHeap, ID3D12Device* _dxDevice) {
	// gpuに送るResourceの作成
	particleResource_ = std::make_unique<ShaderResource>();
	freeListIndexResource_ = std::make_unique<ShaderResource>();
	freeListResource_ = std::make_unique<ShaderResource>();

	D3D12_HEAP_PROPERTIES defaultHeapProperties{};
	defaultHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// particle
	particleResource_->Init(_dxDevice, _dxHeap, CreateUavResourceDesc(sizeof(Particle) * kInstanceNum_),
							&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
	
	// freeListIndex
	freeListIndexResource_->Init(_dxDevice, _dxHeap, CreateUavResourceDesc(sizeof(uint32_t) * kInstanceNum_),
								 &defaultHeapProperties, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);

	// freeList
	freeListResource_->Init(_dxDevice, _dxHeap, CreateUavResourceDesc(sizeof(uint32_t) * kInstanceNum_),
							&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);

	// 各UAV, SRV
	particleResource_->CreateUAV(CreateUavDesc(kInstanceNum_, sizeof(Particle)));
	particleResource_->CreateSRV(CreateSrvDesc(kInstanceNum_, sizeof(Particle)));

	freeListIndexResource_->CreateUAV(CreateUavDesc(kInstanceNum_, sizeof(uint32_t)));
	freeListIndexResource_->CreateSRV(CreateSrvDesc(kInstanceNum_, sizeof(uint32_t)));

	freeListResource_->CreateUAV(CreateUavDesc(kInstanceNum_, sizeof(uint32_t)));
	freeListResource_->CreateSRV(CreateSrvDesc(kInstanceNum_, sizeof(uint32_t)));

	perViewBuffer_ = CreateBufferResource(GraphicsContext::GetInstance()->GetDevice(), sizeof(PerView));
	perViewBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&perView_));

	// ゲーム情報
	perView_->viewProjection = Matrix4x4::MakeUnit();
	perView_->billboardMat = Matrix4x4::MakeUnit();

	perFrameBuffer_ = CreateBufferResource(GraphicsContext::GetInstance()->GetDevice(), sizeof(PerFrame));
	perFrameBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&perFrame_));

	maxParticleBuffer_ = CreateBufferResource(GraphicsContext::GetInstance()->GetDevice(), sizeof(MaxParticles));
	maxParticleBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&maxBuffer_));
	maxBuffer_->count = kInstanceNum_;

}
