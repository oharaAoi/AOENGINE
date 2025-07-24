#include "GlitchNoise.h"
#include "Engine.h"
#include "Engine/Core/GraphicsContext.h"

GlitchNoise::~GlitchNoise() {
	glitchBuffer_->Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GlitchNoise::Init() {
	GraphicsContext* graphicsCtx = GraphicsContext::GetInstance();
	glitchBuffer_ = std::make_unique<DxResource>();
	glitchBuffer_->Init(graphicsCtx->GetDevice(), graphicsCtx->GetDxHeap(), ResourceType::COMMON);
	glitchBuffer_->CreateResource(sizeof(GlitchSetting));
	glitchBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	setting_->texelSize = { 2.0f / (float)kWindowWidth_, 2.0f / (float)kWindowHeight_ };
	setting_->time = 0.0f;
	setting_->strength = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void GlitchNoise::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	if (setting_->strength != 0.0f) {
		setting_->time += GameTimer::DeltaTime();
		setting_->frameIndex++;

		if (setting_->time > noiseTime_) {
			setting_->strength = 0.0f;
		}
	}

	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_GlitchNoise.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gGlitchNoise");
	commandList->SetGraphicsRootConstantBufferView(index, glitchBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ ノイズの設定
///////////////////////////////////////////////////////////////////////////////////////////////

void GlitchNoise::StartNoise(float startStrength, float time) {
	setting_->strength = startStrength;
	noiseTime_ = time;
	setting_->time = 0.0f;
	setting_->frameIndex = 0;
}

void GlitchNoise::CheckBox() {
	ImGui::Checkbox("GlitchNoise##GlitchNoise_checkbox", &isEnable_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Debug表示
///////////////////////////////////////////////////////////////////////////////////////////////

void GlitchNoise::Debug_Gui() {
	if (ImGui::CollapsingHeader("GlitchNoise")) {
		ImGui::DragFloat("time", &noiseTime_, 0.01f);
		
		if (ImGui::Button("AddTime")) {
			setting_->strength = 1.0f;
			setting_->time = 0.0f;
			setting_->frameIndex = 0;
		}
	}
}