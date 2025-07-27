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

void GpuParticleRenderer::Init(uint32_t _instanceNum) {
	// ポインタの取得
	GraphicsContext* graphicsCxt = GraphicsContext::GetInstance();
	DescriptorHeap* dxHeap = graphicsCxt->GetDxHeap();
	ID3D12Device* dxDevice = graphicsCxt->GetDevice();
	//ID3D12GraphicsCommandList* commandList = graphicsCxt->GetCommandList();

	kInstanceNum_ = _instanceNum;

	// GPUへ送るResourceの作成
	CreateResource(dxHeap, dxDevice);

	// meshの作成
	shape_ = std::make_unique<GeometryObject>();
	shape_->Set<PlaneGeometry>();
	shape_->GetMaterial()->SetUseTexture("white.png");

	/*Engine::SetCsPipeline(CsPipelineType::GpuParticleInit);
	commandList->Dispatch((UINT)kInstanceNum_ / 1024, 1, 1);*/
}

void GpuParticleRenderer::Update() {

}

void GpuParticleRenderer::Draw() const {

}

void GpuParticleRenderer::CreateResource(DescriptorHeap* _dxHeap, ID3D12Device* _dxDevice) {
	// gpuに送るResourceの作成
	particleResource_ = std::make_unique<ShaderResource>();
	freeListIndexResource_ = std::make_unique<ShaderResource>();
	freeListResource_ = std::make_unique<ShaderResource>();

	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

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
}
