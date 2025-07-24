#include "BrightnessThreshold.h"
#include "Engine.h"
#include "Engine/Core/GraphicsContext.h"

BrightnessThreshold::~BrightnessThreshold() {
	bloomBuffer_->Finalize();
}

void BrightnessThreshold::Init() {
	GraphicsContext* graphicsCtx = GraphicsContext::GetInstance();
	// 輝度抽出の設定
	bloomBuffer_ = std::make_unique<DxResource>();
	bloomBuffer_->Init(graphicsCtx->GetDevice(), graphicsCtx->GetDxHeap(), ResourceType::COMMON);
	bloomBuffer_->CreateResource(sizeof(BloomSettings));
	bloomBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&bloomSetting_));

	bloomSetting_->threshold = 1.0f;

}

void BrightnessThreshold::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_BrightnessThreshold.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gBloomSettings");
	commandList->SetGraphicsRootConstantBufferView(index, bloomBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

void BrightnessThreshold::CheckBox() {
}

void BrightnessThreshold::Debug_Gui() {
	ImGui::DragFloat("threshold", &bloomSetting_->threshold, 0.01f, 0.0f, 1.0f);
}