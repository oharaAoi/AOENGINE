#include "ToonMap.h"
#include "Engine.h"

using namespace AOENGINE;
using namespace PostEffect;

void ToonMap::Init() {
	AOENGINE::GraphicsContext* graphicsCtx = AOENGINE::GraphicsContext::GetInstance();
	settingBuffer_ = graphicsCtx->CreateDxResource(ResourceType::Common);
	settingBuffer_->CreateResource(sizeof(Parameter));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	setting_->exposure = 0.6f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void ToonMap::SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) {
	ApplySaveSettings();
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_ToonMap.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gParameter");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ チェックボックスの表示
///////////////////////////////////////////////////////////////////////////////////////////////

void ToonMap::CheckBox() {
	ImGui::Checkbox("ToonMap#ToonMap_checkbox", &isEnable_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存項目の適応
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::ToonMap::ApplySaveSettings() {
	setting_->exposure = saveSettings_.exposure;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::ToonMap::Save(const std::string& rootField) {
	saveSettings_.isEnable = isEnable_;
	saveSettings_.SetRootField(rootField);
	saveSettings_.Save();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::ToonMap::Load(const std::string& rootField) {
	saveSettings_.SetRootField(rootField);
	saveSettings_.Load();
	isEnable_ = saveSettings_.isEnable;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::ToonMap::Debug_Gui() {
	if (ImGui::CollapsingHeader("ToonMap##ToonMap_Header")) {
		ImGui::DragFloat("exposure", &saveSettings_.exposure, 0.01f, 0.0f);
	}
}
