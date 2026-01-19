#include "GaussianFilter.h"
#include "Engine.h"

using namespace AOENGINE;
using namespace PostEffect;

GaussianFilter::~GaussianFilter() {
	settingBuffer_->Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GaussianFilter::Init() {
	AOENGINE::GraphicsContext* graphicsCtx = AOENGINE::GraphicsContext::GetInstance();
	settingBuffer_ = graphicsCtx->CreateDxResource(ResourceType::Common);
	settingBuffer_->CreateResource(sizeof(Setting));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	setting_->deviation = 2.f;
	setting_->size = 5;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void GaussianFilter::SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) {
	ApplySaveSettings();
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_GaussianFilter.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gGaussianSetting");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GaussianFilter::CheckBox() {
	ImGui::Checkbox("GaussianFilter##GaussianFilter_checkBox", &isEnable_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存項目の適応
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::GaussianFilter::ApplySaveSettings() {
	setting_->deviation = saveSettings_.deviation;
	setting_->size = saveSettings_.size;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::GaussianFilter::Save(const std::string& rootField) {
	saveSettings_.isEnable = isEnable_;
	saveSettings_.SetRootField(rootField);
	saveSettings_.Save();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::GaussianFilter::Load(const std::string& rootField) {
	saveSettings_.SetRootField(rootField);
	saveSettings_.Load();
	isEnable_ = saveSettings_.isEnable;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::GaussianFilter::Debug_Gui() {
	if (ImGui::CollapsingHeader("GaussianFilter##GaussianFilter_Header")) {
		saveSettings_.Debug_Gui();
	}
}

void PostEffect::GaussianFilter::SaveSettings::Debug_Gui() {
	ImGui::DragFloat("deviation", &deviation);
	ImGui::DragScalar("size", ImGuiDataType_U32, &size, 1, 0);
}