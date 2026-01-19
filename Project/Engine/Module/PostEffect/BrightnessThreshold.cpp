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
	ApplySaveSettings();

	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_BrightnessThreshold.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gBloomSettings");
	commandList->SetGraphicsRootConstantBufferView(index, bloomBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ チェックボックスの表示
///////////////////////////////////////////////////////////////////////////////////////////////

void BrightnessThreshold::CheckBox() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::BrightnessThreshold::Save(const std::string& rootField) {
	saveBloomSettings_.SetRootField(rootField);
	saveBloomSettings_.Save();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::BrightnessThreshold::Load(const std::string& rootField) {
	saveBloomSettings_.SetRootField(rootField);
	saveBloomSettings_.Load();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存項目の適応
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::BrightnessThreshold::ApplySaveSettings() {
	bloomSetting_->threshold = saveBloomSettings_.threshold;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BrightnessThreshold::Debug_Gui() {
	saveBloomSettings_.Debug_Gui();
}

void BrightnessThreshold::SaveBloomSettings::Debug_Gui() {
	ImGui::DragFloat("threshold", &threshold, 0.01f, 0.0f, 1.0f);
}