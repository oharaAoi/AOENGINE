#include "RadialBlur.h"
#include "Engine.h"
#include "Engine/Lib/GameTimer.h"

RadialBlur::~RadialBlur() {
	blurSettingBuffer_->Finalize();
}

void RadialBlur::Init() {
	GraphicsContext* graphicsCtx = GraphicsContext::GetInstance();
	blurSettingBuffer_ = std::make_unique<DxResource>();
	blurSettingBuffer_->Init(graphicsCtx->GetDevice(), graphicsCtx->GetDxHeap(), ResourceType::COMMON);
	blurSettingBuffer_->CreateResource(sizeof(BlurSetting));
	blurSettingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	setting_->blurCenter = { 0.5f, 0.5f };
	setting_->blurStrength = 0.00f;
	setting_->blurStart = 0.2f;
	setting_->sampleCount = 16;

}

void RadialBlur::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	if (run_) {
		blurStrengthTween_.Update(GameTimer::DeltaTime());
		setting_->blurStrength = blurStrengthTween_.GetValue();


		if (blurStrengthTween_.GetIsFinish()) {
			run_ = false;
		}
	}

	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_RadialBlur.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gBlurSetting");
	commandList->SetGraphicsRootConstantBufferView(index, blurSettingBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

void RadialBlur::CheckBox() {
	ImGui::Checkbox("RadialBlur##RadialBlur_checkbox", &isEnable_);
}

void RadialBlur::Start(float strength, float startTime) {
	run_ = true;
	blurStrengthTween_.Init(0.0f, strength, startTime, (int)EasingType::None::Liner, LoopType::ROUNDTRIP);
}

void RadialBlur::Stop(float stopTime) {
	run_ = false;
}

void RadialBlur::Debug_Gui() {
	if (ImGui::CollapsingHeader("RadialBlur")) {
		ImGui::DragFloat2("center", &setting_->blurCenter.x, 0.1f);
		ImGui::DragFloat("strength", &setting_->blurStrength, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("blurStart", &setting_->blurStart, 0.1f, 0.0f, 1.0f);
		ImGui::DragInt("sampleCount", &setting_->sampleCount, 1, 1, 20);

		setting_->blurCenter.Clamp(CVector2::ZERO, CVector2::UNIT);
	}
}