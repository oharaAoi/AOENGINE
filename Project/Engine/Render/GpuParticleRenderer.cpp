#include "GpuParticleRenderer.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Math/Frustum.h"

#include <algorithm>

using namespace AOENGINE;

GpuParticleRenderer::~GpuParticleRenderer() {
	perViewBuffer_.Reset();
	perFrameBuffer_.Reset();
	cullingDataBuffer_.Reset();
	indirectArgsResetBuffer_.Reset();
	drawCommandSignature_.Reset();
	if (particleResource_) { particleResource_->Destroy(); }
	if (freeListIndexResource_) { freeListIndexResource_->Destroy(); }
	if (freeListResource_) { freeListResource_->Destroy(); }
	if (visibleParticleIndexResource_) { visibleParticleIndexResource_->Destroy(); }
	if (indirectArgsResource_) { indirectArgsResource_->Destroy(); }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleRenderer::Init(uint32_t _instanceNum) {
	// ポインタの取得
	AOENGINE::GraphicsContext* graphicsCxt = AOENGINE::GraphicsContext::GetInstance();
	ID3D12GraphicsCommandList* commandList = graphicsCxt->GetCommandList();

	kInstanceNum_ = _instanceNum;
	
	// GPUへ送るResourceの作成
	CreateResource(graphicsCxt->GetDxResourceManager());

	// meshの作成
	shape_ = std::make_unique<GeometryObject>();
	shape_->Set<PlaneGeometry>();
	shape_->GetMaterial()->SetAlbedoTexture("white.png");

	material_ = std::make_unique<Material>();
	material_->Init();
	material_->SetAlbedoTexture("circle.png");

	CreateCullingResource(graphicsCxt->GetDxResourceManager());

	groups_ = (_instanceNum + threads_ - 1) / threads_;

	// 初期化コマンドの実行
	particleResource_->Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	freeListIndexResource_->Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	freeListResource_->Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
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
	commandList->Dispatch(groups_, 1, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleRenderer::Update() {
	perFrame_->deltaTime = AOENGINE::GameTimer::DeltaTime();
	perFrame_->time = AOENGINE::GameTimer::TotalTime();

	ID3D12GraphicsCommandList* commandList = AOENGINE::GraphicsContext::GetInstance()->GetCommandList();
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

	commandList->Dispatch(groups_, 1, 1);

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

void GpuParticleRenderer::Draw(const Math::Frustum* frustum) const {
	ID3D12GraphicsCommandList* commandList = AOENGINE::GraphicsContext::GetInstance()->GetCommandList();

	cullingData_->useFrustumCulling = frustum != nullptr ? 1u : 0u;
	if (frustum != nullptr) {
		const auto& planes = frustum->GetPlanes();
		for (size_t index = 0; index < planes.size(); ++index) {
			cullingData_->frustumPlanes[index] = Math::Vector4{
				planes[index].normal.x,
				planes[index].normal.y,
				planes[index].normal.z,
				planes[index].distance
			};
		}
	}

	// Indirect引数のInstanceCountを0へ戻してから、可視ParticleをGPU上で詰めます。
	indirectArgsResource_->Transition(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
	commandList->CopyBufferRegion(
		indirectArgsResource_->GetResource(), 0,
		indirectArgsResetBuffer_.Get(), 0,
		sizeof(D3D12_DRAW_INDEXED_ARGUMENTS));
	indirectArgsResource_->Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	visibleParticleIndexResource_->Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	particleResource_->Transition(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	Engine::SetPipelineCS("GpuParticleCull.json");
	Pipeline* cullingPso = Engine::GetLastUsedPipelineCS();
	UINT index = cullingPso->GetRootSignatureIndex("gParticles");
	commandList->SetComputeRootDescriptorTable(index, particleResource_->GetSRV().handleGPU);
	index = cullingPso->GetRootSignatureIndex("gVisibleParticleIndices");
	commandList->SetComputeRootDescriptorTable(index, visibleParticleIndexResource_->GetUAV().handleGPU);
	index = cullingPso->GetRootSignatureIndex("gIndirectArgs");
	commandList->SetComputeRootDescriptorTable(index, indirectArgsResource_->GetUAV().handleGPU);
	index = cullingPso->GetRootSignatureIndex("gCulling");
	commandList->SetComputeRootConstantBufferView(index, cullingDataBuffer_->GetGPUVirtualAddress());
	commandList->Dispatch(groups_, 1, 1);

	visibleParticleIndexResource_->Transition(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	indirectArgsResource_->Transition(commandList, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);

	Engine::SetPipeline(PSOType::Object3d, "Object_GpuParticle.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();

	commandList->IASetVertexBuffers(0, 1, &shape_->GetMesh()->GetVBV());
	commandList->IASetIndexBuffer(&shape_->GetMesh()->GetIBV());

	index = pso->GetRootSignatureIndex("gMaterial");
	commandList->SetGraphicsRootConstantBufferView(index, material_->GetBufferAddress());
	index = pso->GetRootSignatureIndex("gParticles");
	commandList->SetGraphicsRootDescriptorTable(index, particleResource_->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gVisibleParticleIndices");
	commandList->SetGraphicsRootDescriptorTable(index, visibleParticleIndexResource_->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gTexture");
	std::string textureName = material_->GetAlbedoTexture();
	AOENGINE::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureName, index);
	index = pso->GetRootSignatureIndex("gPerView");
	commandList->SetGraphicsRootConstantBufferView(index, perViewBuffer_->GetGPUVirtualAddress());

	commandList->ExecuteIndirect(
		drawCommandSignature_.Get(),
		1,
		indirectArgsResource_->GetResource(),
		0,
		nullptr,
		0);

	// 次のViewまたは次フレームのCompute更新で再利用できる状態へ戻します。
	visibleParticleIndexResource_->Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	indirectArgsResource_->Transition(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
	particleResource_->Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Resourceの作成
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleRenderer::CreateResource(AOENGINE::DxResourceManager* _resourceManager) {
	// gpuに送るResourceの作成
	particleResource_ = _resourceManager->CreateResource(ResourceType::Common);
	freeListIndexResource_ = _resourceManager->CreateResource(ResourceType::Common);
	freeListResource_ = _resourceManager->CreateResource(ResourceType::Common);

	D3D12_HEAP_PROPERTIES defaultHeapProperties{};
	defaultHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// descの作成
	D3D12_RESOURCE_DESC resourceDesc = CreateUavResourceDesc(sizeof(Particle) * kInstanceNum_);
	D3D12_RESOURCE_DESC freeListIndexDesc = CreateUavResourceDesc(sizeof(uint32_t) * kInstanceNum_);
	D3D12_RESOURCE_DESC freeListDesc = CreateUavResourceDesc(sizeof(uint32_t) * kInstanceNum_);

	// particle
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

	maxParticleBuffer_ = CreateBufferResource(AOENGINE::GraphicsContext::GetInstance()->GetDevice(), sizeof(MaxParticles));
	maxParticleBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&maxBuffer_));
	maxBuffer_->count = kInstanceNum_;

}

void GpuParticleRenderer::CreateCullingResource(AOENGINE::DxResourceManager* resourceManager) {
	ID3D12Device* device = AOENGINE::GraphicsContext::GetInstance()->GetDevice();

	D3D12_HEAP_PROPERTIES defaultHeapProperties{};
	defaultHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	visibleParticleIndexResource_ = resourceManager->CreateResource(ResourceType::Common);
	D3D12_RESOURCE_DESC visibleIndexDesc = CreateUavResourceDesc(sizeof(uint32_t) * kInstanceNum_);
	visibleParticleIndexResource_->CreateResource(
		&visibleIndexDesc,
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		D3D12_RESOURCE_STATE_COMMON);
	visibleParticleIndexResource_->CreateUAV(CreateUavDesc(kInstanceNum_, sizeof(uint32_t)));
	visibleParticleIndexResource_->CreateSRV(CreateSrvDesc(kInstanceNum_, sizeof(uint32_t)));

	indirectArgsResource_ = resourceManager->CreateResource(ResourceType::Common);
	D3D12_RESOURCE_DESC indirectArgsDesc = CreateUavResourceDesc(sizeof(D3D12_DRAW_INDEXED_ARGUMENTS));
	indirectArgsResource_->CreateResource(
		&indirectArgsDesc,
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		D3D12_RESOURCE_STATE_COMMON);

	D3D12_UNORDERED_ACCESS_VIEW_DESC indirectArgsUavDesc{};
	indirectArgsUavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	indirectArgsUavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	indirectArgsUavDesc.Buffer.NumElements = sizeof(D3D12_DRAW_INDEXED_ARGUMENTS) / sizeof(uint32_t);
	indirectArgsUavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
	indirectArgsResource_->CreateUAV(indirectArgsUavDesc);

	indirectArgsResetBuffer_ = CreateBufferResource(device, sizeof(D3D12_DRAW_INDEXED_ARGUMENTS));
	D3D12_DRAW_INDEXED_ARGUMENTS* resetArgs = nullptr;
	indirectArgsResetBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&resetArgs));
	*resetArgs = D3D12_DRAW_INDEXED_ARGUMENTS{
		.IndexCountPerInstance = shape_->GetMesh()->GetIndexNum(),
		.InstanceCount = 0,
		.StartIndexLocation = 0,
		.BaseVertexLocation = 0,
		.StartInstanceLocation = 0
	};
	indirectArgsResetBuffer_->Unmap(0, nullptr);

	cullingDataBuffer_ = CreateBufferResource(device, sizeof(CullingData));
	cullingDataBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&cullingData_));
	cullingData_->maxParticles = kInstanceNum_;

	float particleLocalRadius = 0.0f;
	for (const VertexData& vertex : shape_->GetMesh()->GetVerticesData()) {
		const Math::Vector3 position{ vertex.pos.x, vertex.pos.y, vertex.pos.z };
		particleLocalRadius = (std::max)(particleLocalRadius, position.Length());
	}
	cullingData_->particleLocalRadius = particleLocalRadius;

	D3D12_INDIRECT_ARGUMENT_DESC argumentDesc{};
	argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

	D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc{};
	commandSignatureDesc.ByteStride = sizeof(D3D12_DRAW_INDEXED_ARGUMENTS);
	commandSignatureDesc.NumArgumentDescs = 1;
	commandSignatureDesc.pArgumentDescs = &argumentDesc;

	const HRESULT hr = device->CreateCommandSignature(
		&commandSignatureDesc,
		nullptr,
		IID_PPV_ARGS(&drawCommandSignature_));
	assert(SUCCEEDED(hr));
}
