#include "GpuParticleEmitter.h"
#include "Engine/Engine.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Json/JsonItems.h"

GpuParticleEmitter::~GpuParticleEmitter() {
	emitterResource_.Reset();
	perFrameBuffer_.Reset();
}

void GpuParticleEmitter::Debug_Gui() {
	ImGui::Text("emitAccumulator : %f", emitAccumulator_);
	emitterItem_.Attribute_Gui();
	if (ImGui::Button("Save")) {
		JsonItems::Save("GPU", emitterItem_.ToJson(GetName()), "Effect");
	}
	if (ImGui::Button("Apply")) {
		emitterItem_.FromJson(JsonItems::GetData("GPU", GetName()));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleEmitter::Init(const std::string& name) {
	GraphicsContext* ctx = GraphicsContext::GetInstance();
	ID3D12Device* dxDevice = ctx->GetDevice();
	
	SetName(name.c_str());
	// Resourceの作成
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC resourceDesc = {};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeof(GpuParticleSingleData);
	// バッファの場合がこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	// バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	emitterResource_ = CreateBufferResource(dxDevice, sizeof(GpuParticleSingleData));
	emitterResource_->Map(0, nullptr, reinterpret_cast<void**>(&gpuData));

	// preFrameの作成
	perFrameBuffer_ = CreateBufferResource(dxDevice, sizeof(PerFrame));
	perFrameBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&perFrame_));

	emitterItem_.FromJson(JsonItems::GetData("GpuParticle", name));

	SetItem();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleEmitter::Update() {
	perFrame_->deltaTime = GameTimer::DeltaTime();
	perFrame_->time = GameTimer::TotalTime();

	GraphicsContext* ctx = GraphicsContext::GetInstance();
	ID3D12GraphicsCommandList* commandList = ctx->GetCommandList();

	// -------------------------------------------------
	// ↓ 発射処理
	// -------------------------------------------------
	if (!emitterItem_.isLoop) {
		for (uint32_t count = 0; count < emitterItem_.rateOverTimeCout; ++count) {
			EmitCommand(commandList);
		}
		isStop_ = true;
	}

	emitAccumulator_ += emitterItem_.rateOverTimeCout * GameTimer::DeltaTime();
	// 発射すべき個数を計算する
	int emitCout = static_cast<int>(emitAccumulator_);
	gpuData->count = emitCout;
	if (emitCout != 0) {
		EmitCommand(commandList);
	}
	emitAccumulator_ -= emitCout;

	// -------------------------------------------------
	// ↓ 継続時間を進める
	// -------------------------------------------------
	currentTimer_ += GameTimer::DeltaTime();
	if (currentTimer_ > emitterItem_.duration) {
		if (!emitterItem_.isLoop) {
			isStop_ = true;
		}
	}

	SetItem();
}

void GpuParticleEmitter::EmitCommand(ID3D12GraphicsCommandList* commandList) {
	Engine::SetPipelineCS("GpuParticleEmit.json");
	Pipeline* pso = Engine::GetLastUsedPipelineCS();
	UINT index = 0;
	index = pso->GetRootSignatureIndex("gParticles");
	commandList->SetComputeRootDescriptorTable(index, particleResourceHandle_);
	index = pso->GetRootSignatureIndex("gFreeListIndex");
	commandList->SetComputeRootDescriptorTable(index, freeListIndexHandle_);
	index = pso->GetRootSignatureIndex("gFreeList");
	commandList->SetComputeRootDescriptorTable(index, freeListHandle_);
	index = pso->GetRootSignatureIndex("gPerFrame");
	commandList->SetComputeRootConstantBufferView(index, perFrameBuffer_->GetGPUVirtualAddress());
	index = pso->GetRootSignatureIndex("gMaxParticles");
	commandList->SetComputeRootConstantBufferView(index, maxParticleResource_->GetGPUVirtualAddress());
	index = pso->GetRootSignatureIndex("gEmitter");
	commandList->SetComputeRootConstantBufferView(index, emitterResource_->GetGPUVirtualAddress());
	commandList->Dispatch(1, 1, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ パラメータを設定する
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleEmitter::SetItem() {
	gpuData->color = emitterItem_.color;
	gpuData->minScale = emitterItem_.minScale;
	gpuData->maxScale = emitterItem_.maxScale;
	gpuData->targetScale = emitterItem_.targetScale;
	gpuData->rotate = emitterItem_.rotate;
	gpuData->pos = emitterItem_.pos;
	gpuData->count = emitterItem_.rateOverTimeCout;
	gpuData->emitType = emitterItem_.emitType;
	gpuData->shape = emitterItem_.shape;
	gpuData->lifeOfScaleDown = emitterItem_.lifeOfScaleDown;
	gpuData->lifeOfScaleUp = emitterItem_.lifeOfScaleUp;
	gpuData->lifeOfAlpha = emitterItem_.lifeOfAlpha;

	gpuData->separateByAxisScale = emitterItem_.separateByAxisScale;
	gpuData->scaleMinScaler = emitterItem_.scaleMinScaler;
	gpuData->scaleMaxScaler = emitterItem_.scaleMaxScaler;


	gpuData->speed = emitterItem_.speed;
	gpuData->lifeTime = emitterItem_.lifeTime;
	gpuData->gravity = emitterItem_.gravity;
	gpuData->damping = emitterItem_.damping;
}

void GpuParticleEmitter::Emit() {
	if (isStop_) { return; }
}

void GpuParticleEmitter::EmitUpdate() {
	if (isStop_) { return; }
}