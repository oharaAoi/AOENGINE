#include "GaussianBlurWidth.h"
#include "Engine.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/WinApp/WinApp.h"

using namespace AOENGINE;
using namespace PostEffect;

GaussianBlurWidth::~GaussianBlurWidth() {
	blurBuffer_->Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GaussianBlurWidth::Init() {
	AOENGINE::GraphicsContext* graphicsCtx = AOENGINE::GraphicsContext::GetInstance();
	// blurの設定
	blurBuffer_ = graphicsCtx->CreateDxResource(ResourceType::Common);
	blurBuffer_->CreateResource(sizeof(BlurSettings));
	blurBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&blurSetting_));

	blurSetting_->texelSize = { 1.0f / (float)WinApp::sClientWidth, 1.0f / (float)WinApp::sClientHeight };
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void GaussianBlurWidth::SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) {
	ApplySaveSettings();
	// blur
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_GaussianBlurWidth.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gBlurSettings");
	commandList->SetGraphicsRootConstantBufferView(index, blurBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ チェックボックスの表示
///////////////////////////////////////////////////////////////////////////////////////////////

void GaussianBlurWidth::CheckBox() {
	ImGui::Checkbox("GaussianBlurWidth", &isEnable_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存項目の適応
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::GaussianBlurWidth::ApplySaveSettings() {
	blurSetting_->texelSize = {
		saveSettings_.sampleCount / (float)WinApp::sClientWidth,
		saveSettings_.sampleCount / (float)WinApp::sClientHeight
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::GaussianBlurWidth::Save(const std::string& rootField) {
	saveSettings_.SetRootField(rootField);
	saveSettings_.Save();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::GaussianBlurWidth::Load(const std::string& rootField) {
	saveSettings_.SetRootField(rootField);
	saveSettings_.Load();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GaussianBlurWidth::Debug_Gui() {
	saveSettings_.Debug_Gui();
	blurSetting_->texelSize = {
		saveSettings_.sampleCount / (float)WinApp::sClientWidth,
		saveSettings_.sampleCount / (float)WinApp::sClientHeight
	};
}

void PostEffect::GaussianBlurWidth::SaveSettings::Debug_Gui() {
	ImGui::DragScalar("sampleHeight", ImGuiDataType_U32, &sampleCount, 1, 0);
}