#include "RadialBlur.h"
#include "Engine.h"

RadialBlur::~RadialBlur() {
	blurSettingBuffer_->Finalize();
}

void RadialBlur::Init() {
	blurSettingBuffer_ = std::make_unique<DxResource>();
	blurSettingBuffer_->Init(Engine::GetDevice(), Engine::GetDxHeap(),ResourceType::COMMON);
	blurSettingBuffer_->CreateResource(sizeof(BlurSetting));
	blurSettingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	setting_->blurCenter = { 0.5f, 0.5f };
	setting_->blurStrength = 0.02f;
	setting_->blurStart = 0.2f;
	setting_->sampleCount = 16;
}

void RadialBlur::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	Engine::SetPSOProcessed(ProcessedScenePSO::RadialBlur);
	commandList->SetGraphicsRootDescriptorTable(0, pingResource->GetSRV().handleGPU);
	commandList->SetGraphicsRootConstantBufferView(1, blurSettingBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

#ifdef _DEBUG
void RadialBlur::Debug_Gui() {
	if (ImGui::CollapsingHeader("RadialBlur")) {
		ImGui::DragFloat2("center", &setting_->blurCenter.x, 0.1f);
		ImGui::DragFloat("strength", &setting_->blurStrength, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("blurStart", &setting_->blurStart, 0.1f, 0.0f, 1.0f);
		ImGui::DragInt("sampleCount", &setting_->sampleCount, 1, 1, 20);

		setting_->blurCenter.Clamp(CVector2::ZERO, CVector2::UNIT);
	}
}
#endif