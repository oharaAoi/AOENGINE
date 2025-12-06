#include "BrightnessThreshold.h"
#include "Engine.h"
#include "Engine/Core/GraphicsContext.h"

using namespace AOENGINE;
using namespace PostEffect;

BrightnessThreshold::~BrightnessThreshold() {
	bloomBuffer_->Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BrightnessThreshold::Init() {
	AOENGINE::GraphicsContext* graphicsCtx = AOENGINE::GraphicsContext::GetInstance();
	// 輝度抽出の設定
	bloomBuffer_ = graphicsCtx->CreateDxResource(ResourceType::Common);
	bloomBuffer_->CreateResource(sizeof(BloomSettings));
	bloomBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&bloomSetting_));

	bloomSetting_->threshold = 1.0f;

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void BrightnessThreshold::SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) {
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_BrightnessThreshold.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gBloomSettings");
	commandList->SetGraphicsRootConstantBufferView(index, bloomBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BrightnessThreshold::CheckBox() {
}

void BrightnessThreshold::Debug_Gui() {
	ImGui::DragFloat("threshold", &bloomSetting_->threshold, 0.01f, 0.0f, 1.0f);
}