#include "Grayscale.h"
#include "Engine.h"

Grayscale::~Grayscale() {
	
}

void Grayscale::Init() {
	resource_ = std::make_unique<DxResource>();

}

void Grayscale::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	Engine::SetPSOProcessed(ProcessedScenePSO::Grayscale);
	commandList->SetGraphicsRootDescriptorTable(0, pingResource->GetSRV().handleGPU);
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}