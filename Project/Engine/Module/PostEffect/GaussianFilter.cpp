#include "GaussianFilter.h"
#include "Engine.h"

GaussianFilter::~GaussianFilter() {
}

void GaussianFilter::Init() {
	GraphicsContext* graphicsCtx = GraphicsContext::GetInstance();
	settingBuffer_ = std::make_unique<DxResource>();
	settingBuffer_->Init(graphicsCtx->GetDevice(), graphicsCtx->GetDxHeap(), ResourceType::COMMON);
	settingBuffer_->CreateResource(sizeof(Setting));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	setting_->deviation = 2.f;
	setting_->size = 5;
}

void GaussianFilter::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_GaussianFilter.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gGaussianSetting");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

void GaussianFilter::CheckBox() {
	ImGui::Checkbox("GaussianFilter##GaussianFilter_checkBox", &isEnable_);
}
