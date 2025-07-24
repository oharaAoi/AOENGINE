#include "Grayscale.h"
#include "Engine.h"

Grayscale::~Grayscale() {
	
}

void Grayscale::Init() {
}

void Grayscale::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_Grayscale.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

void Grayscale::CheckBox() {
	ImGui::Checkbox("GrayScale##Grayscale_checkBox", &isEnable_);
}
