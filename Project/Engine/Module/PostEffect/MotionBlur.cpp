#include "MotionBlur.h"
#include "Engine.h"

MotionBlur::~MotionBlur() {
}

void MotionBlur::Init() {
}

void MotionBlur::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	motionResource_->Transition(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_MotionBlur.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gMotion");
	commandList->SetGraphicsRootDescriptorTable(index, motionResource_->GetSRV().handleGPU);
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);

	motionResource_->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void MotionBlur::CheckBox() {
	ImGui::Checkbox("MotionBlur##MotionBlur_checkbox", &isEnable_);
}
