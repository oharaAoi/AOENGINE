#include "Grayscale.h"
#include "Engine.h"
#include "Engine/Core/GraphicsContext.h"

using namespace AOENGINE;
using namespace PostEffect;

Grayscale::~Grayscale() {
	settingBuffer_->Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Grayscale::Init() {
	AOENGINE::GraphicsContext* graphicsCtx = AOENGINE::GraphicsContext::GetInstance();
	settingBuffer_ = graphicsCtx->CreateDxResource(ResourceType::Common);
	settingBuffer_->CreateResource(sizeof(Setting));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	setting_->color = Colors::Linear::black;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void Grayscale::SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) {
	ApplySaveSettings();
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_Grayscale.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gSetting");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ チェックボックスの表示
///////////////////////////////////////////////////////////////////////////////////////////////

void Grayscale::CheckBox() {
	ImGui::Checkbox("GrayScale##Grayscale_checkBox", &isEnable_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::Grayscale::Save(const std::string& rootField) {
	saveSettings_.isEnable = isEnable_;
	saveSettings_.SetRootField(rootField);
	saveSettings_.Save();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::Grayscale::Load(const std::string& rootField) {
	saveSettings_.SetRootField(rootField);
	saveSettings_.Load();
	isEnable_ = saveSettings_.isEnable;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存項目の適応
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::Grayscale::ApplySaveSettings() {
	setting_->color = saveSettings_.color;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Grayscale::Debug_Gui() {
	if (ImGui::CollapsingHeader("GrayScale##Grayscale_Header")) {
		saveSettings_.Debug_Gui();
	}
}

void Grayscale::SaveSettings::Debug_Gui() {
	ImGui::ColorEdit4("color", &color.r);
}