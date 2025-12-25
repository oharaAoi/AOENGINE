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
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_ToonMap.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gParameter");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

void ToonMap::CheckBox() {
	ImGui::Checkbox("ToonMap#ToonMap_checkbox", &isEnable_);
}

void PostEffect::ToonMap::Debug_Gui() {
	if (ImGui::CollapsingHeader("ToonMap##ToonMap_Header")) {
		ImGui::DragFloat("exposure", &setting_->exposure, 0.1f, 0.0f);
	}
}
