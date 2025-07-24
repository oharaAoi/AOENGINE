#include "Vignette.h"
#include "Engine.h"
#include "Engine/Core/GraphicsContext.h"

Vignette::~Vignette() {
	settingBuffer_->Finalize();
}

void Vignette::Init() {
	GraphicsContext* graphicsCtx = GraphicsContext::GetInstance();
	settingBuffer_ = std::make_unique<DxResource>();
	settingBuffer_->Init(graphicsCtx->GetDevice(), graphicsCtx->GetDxHeap(), ResourceType::COMMON);
	settingBuffer_->CreateResource(sizeof(VignetteSetting));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	setting_->color = Vector4(0, 0, 0, 1.0f);
	setting_->scale = 16.0f;
	setting_->power = 0.8f;
}

void Vignette::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_Vignette.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gSetting");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

void Vignette::CheckBox() {
	ImGui::Checkbox("Vignette##Vignette_checkbox", &isEnable_);
}

void Vignette::Debug_Gui() {
	if (ImGui::CollapsingHeader("Vignette")) {
		ImGui::ColorEdit4("color", &setting_->color.x);
		ImGui::DragFloat("scale", &setting_->scale, 0.1f);
		ImGui::DragFloat("power", &setting_->power, 0.01f);

		setting_->scale = std::clamp(setting_->scale, 0.0f, 20.0f);
		setting_->power = std::clamp(setting_->power, 0.0f, 1.0f);
	}
}