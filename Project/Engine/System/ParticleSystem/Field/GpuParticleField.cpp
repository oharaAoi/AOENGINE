#include "GpuParticleField.h"
#include "Engine/Engine.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Lib/GameTimer.h"

using namespace AOENGINE;

GpuParticleField::~GpuParticleField() {

}

void GpuParticleField::Debug_Gui() {
	ImGui::DragFloat3("acceleration", &fieldData_->acceleration.x, 0.1f);
	ImGui::DragFloat3("min", &fieldData_->min.x, 0.1f);
	ImGui::DragFloat3("max", &fieldData_->max.x, 0.1f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleField::Init(uint32_t _instanceNum) {
	AOENGINE::GraphicsContext* ctx = AOENGINE::GraphicsContext::GetInstance();
	ID3D12Device* dxDevice = ctx->GetDevice();

	kInstanceNum_ = _instanceNum;

	fieldBuffer_ = CreateBufferResource(dxDevice, sizeof(AccelerationField));
	fieldBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&fieldData_));

	perFrameBuffer_ = CreateBufferResource(dxDevice, sizeof(PerFrame));
	perFrameBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&perFrame_));

	fieldData_->acceleration = { -10.0f, 0.0f, 0.0f };
	fieldData_->max = { 1000, 1000, 1000 };
	fieldData_->min = { -1000, -1000, -1000 };

	isEnable_ = true;

	EditorWindows::AddObjectWindow(this, "GpuParticleField");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleField::Update() {
	perFrame_->deltaTime = GameTimer::DeltaTime();
	perFrame_->time = GameTimer::TotalTime();
	if (!isEnable_) { return; }
	
	timer_ += GameTimer::DeltaTime();
}

void GpuParticleField::DrawShape() const {

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行
///////////////////////////////////////////////////////////////////////////////////////////////

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
