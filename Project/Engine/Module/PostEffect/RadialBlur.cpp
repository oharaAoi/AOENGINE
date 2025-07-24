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

	stop_ = false;
}

void RadialBlur::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	if (start_) {
		timer_ += GameTimer::DeltaTime();
		float t = timer_ / startTime_;
		setting_->blurStrength = std::lerp(0.0f, preStrength_, t);

		if (timer_ > startTime_) {
			start_ = false;
		}
	}

	if (stop_) {
		timer_ += GameTimer::DeltaTime();
		float t = timer_ / stopTime_;
		setting_->blurStrength = std::lerp(preStrength_, 0.0f, t);

		if (timer_ > stopTime_) {
			stop_ = false;
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
	start_ = true;
	stop_ = false;
	setting_->blurStrength = strength;
	startTime_ = startTime;
	preStrength_ = strength;
	timer_ = 0.0f;
}

void RadialBlur::Stop(float stopTime) {
	start_ = false;
	stop_ = true;
	stopTime_ = stopTime;
	timer_ = 0.0f;
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