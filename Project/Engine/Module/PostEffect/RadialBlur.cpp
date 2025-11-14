#include "RadialBlur.h"
#include "Engine.h"
#include "Engine/Lib/GameTimer.h"

RadialBlur::~RadialBlur() {
	blurSettingBuffer_->Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void RadialBlur::Init() {
	GraphicsContext* graphicsCtx = GraphicsContext::GetInstance();
	blurSettingBuffer_ = graphicsCtx->CreateDxResource(ResourceType::COMMON);
	blurSettingBuffer_->CreateResource(sizeof(BlurSetting));
	blurSettingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	setting_->blurCenter = { 0.5f, 0.5f };
	setting_->blurStrength = 0.00f;
	setting_->blurStart = 0.2f;
	setting_->sampleCount = 16;

	continuation_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void RadialBlur::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	if (run_) {
		blurStrengthTween_.Update(GameTimer::DeltaTime());
		setting_->blurStrength = blurStrengthTween_.GetValue();

		if (!continuation_) {
			if (blurStrengthTween_.GetIsFinish()) {
				Stop();
			}
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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void RadialBlur::CheckBox() {
	ImGui::Checkbox("RadialBlur##RadialBlur_checkbox", &isEnable_);
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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行関数
///////////////////////////////////////////////////////////////////////////////////////////////

void RadialBlur::Start(float _strength, float _startTime, bool _continuation) {
	run_ = true;
	continuation_ = _continuation;
	blurStrengthTween_.Init(0.0f, _strength, _startTime, (int)EasingType::None::Liner, LoopType::STOP);
}

void RadialBlur::Stop() {
	run_ = false;
	continuation_ = false;
	setting_->blurStrength = 0.0f;
}

void RadialBlur::SlowDown(float time) {
	run_ = true;
	blurStrengthTween_.Init(setting_->blurStrength, 0.0f, time, (int)EasingType::None::Liner, LoopType::STOP);
}