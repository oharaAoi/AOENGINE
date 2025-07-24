#include "Grayscale.h"
#include "Engine.h"
#include "Engine/Core/GraphicsContext.h"

Grayscale::~Grayscale() {
	settingBuffer_->Finalize();
}

void Grayscale::Init() {
	GraphicsContext* graphicsCtx = GraphicsContext::GetInstance();
	settingBuffer_ = std::make_unique<DxResource>();
	settingBuffer_->Init(graphicsCtx->GetDevice(), graphicsCtx->GetDxHeap(), ResourceType::COMMON);
	settingBuffer_->CreateResource(sizeof(Setting));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	setting_->color = Vector4(0, 0, 0, 1.0f);
}

void Grayscale::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_Grayscale.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gSetting");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

void Grayscale::CheckBox() {
	ImGui::Checkbox("GrayScale##Grayscale_checkBox", &isEnable_);
}

void Grayscale::Debug_Gui() {
	if (ImGui::CollapsingHeader("GrayScale##Grayscale_Header")) {
		ImGui::ColorEdit4("color", &setting_->color.x);
	}
}
