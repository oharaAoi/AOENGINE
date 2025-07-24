#include "LuminanceBasedOutline.h"
#include "Engine.h"

void LuminanceBasedOutline::Init() {
}

void LuminanceBasedOutline::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_LuminanceBasedOutline.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

void LuminanceBasedOutline::CheckBox() {
	ImGui::Checkbox("LuminanceBasedOutline##LuminanceBasedOutline_checkbox", &isEnable_);
}
