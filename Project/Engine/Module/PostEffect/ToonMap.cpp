#include "ToonMap.h"
#include "Engine.h"

void ToonMap::Init() {
}

void ToonMap::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_ToonMap.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

void ToonMap::CheckBox() {
	ImGui::Checkbox("ToonMap#ToonMap_checkbox", &isEnable_);
}
