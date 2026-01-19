#include "LuminanceBasedOutline.h"
#include "Engine.h"

using namespace AOENGINE;
using namespace PostEffect;

void LuminanceBasedOutline::Init() {
	AOENGINE::GraphicsContext* graphicsCtx = AOENGINE::GraphicsContext::GetInstance();
	settingBuffer_ = graphicsCtx->CreateDxResource(ResourceType::Common);
	settingBuffer_->CreateResource(sizeof(Setting));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void LuminanceBasedOutline::SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) {
	ApplySaveSettings();
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_LuminanceBasedOutline.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ チェックボックスの表示
///////////////////////////////////////////////////////////////////////////////////////////////

void LuminanceBasedOutline::CheckBox() {
	ImGui::Checkbox("LuminanceBasedOutline##LuminanceBasedOutline_checkbox", &isEnable_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存項目の適応
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::LuminanceBasedOutline::ApplySaveSettings() {
	setting_->scale = saveSettings_.scale;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::LuminanceBasedOutline::Save(const std::string& rootField) {
	saveSettings_.isEnable = isEnable_;
	saveSettings_.SetRootField(rootField);
	saveSettings_.Save();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::LuminanceBasedOutline::Load(const std::string& rootField) {
	saveSettings_.SetRootField(rootField);
	saveSettings_.Load();
	isEnable_ = saveSettings_.isEnable;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::LuminanceBasedOutline::Debug_Gui() {
	if (ImGui::CollapsingHeader("LuminanceBasedOutline##LuminanceBasedOutline_Header")) {
		saveSettings_.Debug_Gui();
	}
}

void PostEffect::LuminanceBasedOutline::SaveSetting::Debug_Gui() {
	ImGui::DragFloat("scale", &scale, 1.f);
}