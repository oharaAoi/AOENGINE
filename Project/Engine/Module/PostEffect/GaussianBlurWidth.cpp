#include "GaussianBlurWidth.h"
#include "Engine.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/WinApp/WinApp.h"

using namespace AOENGINE;

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

	blurSetting_->texelSize = { 1.0f / (float)WinApp::sWindowWidth, 1.0f / (float)WinApp::sWindowHeight };
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void GaussianBlurWidth::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
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
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GaussianBlurWidth::CheckBox() {
	ImGui::Checkbox("GaussianBlurWidth", &isEnable_);
}

void GaussianBlurWidth::Debug_Gui() {
	static float sample = 1;
	ImGui::DragFloat("sampleWide", &sample, 0.1f, 0.0f, 10.0f);
	blurSetting_->texelSize = { sample / (float)WinApp::sWindowWidth, sample / (float)WinApp::sWindowHeight };
}