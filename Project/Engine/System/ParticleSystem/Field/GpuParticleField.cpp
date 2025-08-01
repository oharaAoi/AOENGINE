#include "GpuParticleField.h"
#include "Engine/Engine.h"
#include "Engine/Render.h"

GpuParticleField::~GpuParticleField() {

}

void GpuParticleField::Init(uint32_t _instanceNum) {
	GraphicsContext* ctx = GraphicsContext::GetInstance();
	ID3D12Device* dxDevice = ctx->GetDevice();

	kInstanceNum_ = _instanceNum;

	fieldBuffer_ = CreateBufferResource(dxDevice, sizeof(AccelerationField));
	fieldBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&fieldData_));

	perFrameBuffer_ = CreateBufferResource(dxDevice, sizeof(PerFrame));
	perFrameBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&perFrame_));

	fieldData_->acceleration = { -1.0f, 0.0f, 0.0f };
	fieldData_->max = { 1000, 1000, 1000 };
	fieldData_->min = { -1000, -1000, -1000 };

	isEnable_ = true;
}

void GpuParticleField::Update() {
	perFrame_->deltaTime = GameTimer::DeltaTime();
	perFrame_->time = GameTimer::TotalTime();
	if (!isEnable_) { return; }
	

}

void GpuParticleField::DrawShape() const {

}

void GpuParticleField::Execute(ID3D12GraphicsCommandList* commandList) {
	perFrame_->deltaTime = GameTimer::DeltaTime();
	perFrame_->time = GameTimer::TotalTime();

	const UINT threadsPerGroup = 256;
	const UINT groupCount = (kInstanceNum_ + threadsPerGroup - 1) / threadsPerGroup;


	if (!isEnable_) { return; }
	Engine::SetPipelineCS("AccelerationField.json");
	Pipeline* pso = Engine::GetLastUsedPipelineCS();
	UINT index = 0;
	index = pso->GetRootSignatureIndex("gParticles");
	commandList->SetComputeRootDescriptorTable(index, particleResourceHandle_);
	index = pso->GetRootSignatureIndex("gMaxParticles");
	commandList->SetComputeRootConstantBufferView(index, maxParticleResource_->GetGPUVirtualAddress());
	
	index = pso->GetRootSignatureIndex("gAccelerationField");
	commandList->SetComputeRootConstantBufferView(index, fieldBuffer_->GetGPUVirtualAddress());
	commandList->Dispatch(groupCount, 1, 1);
}
