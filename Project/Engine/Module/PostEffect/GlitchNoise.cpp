#include "GlitchNoise.h"
#include "Engine.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/Lib/GameTimer.h"

using namespace AOENGINE;
using namespace PostEffect;

GlitchNoise::~GlitchNoise() {
	glitchBuffer_->Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GlitchNoise::Init() {
	AOENGINE::GraphicsContext* graphicsCtx = AOENGINE::GraphicsContext::GetInstance();
	glitchBuffer_ = graphicsCtx->CreateDxResource(ResourceType::Common);
	glitchBuffer_->CreateResource(sizeof(GlitchSetting));
	glitchBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	setting_->texelSize = { 2.0f / (float)WinApp::sClientWidth, 2.0f / (float)WinApp::sClientHeight};
	setting_->time = 0.0f;
	setting_->strength = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void GlitchNoise::SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) {
	if (setting_->strength != 0.0f) {
		setting_->time += AOENGINE::GameTimer::DeltaTime();
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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ チェックボックスの表示
///////////////////////////////////////////////////////////////////////////////////////////////

void GlitchNoise::CheckBox() {
	ImGui::Checkbox("GlitchNoise##GlitchNoise_checkbox", &isEnable_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存項目の適応
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::GlitchNoise::ApplySaveSettings() {
	setting_->texelSize = { saveSettings_.texelSize.x / (float)WinApp::sClientWidth, saveSettings_.texelSize.y / (float)WinApp::sClientHeight };
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::GlitchNoise::Save(const std::string& rootField) {
	saveSettings_.isEnable = isEnable_;
	saveSettings_.SetRootField(rootField);
	saveSettings_.Save();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::GlitchNoise::Load(const std::string& rootField) {
	saveSettings_.SetRootField(rootField);
	saveSettings_.Load();
	isEnable_ = saveSettings_.isEnable;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Debug表示
///////////////////////////////////////////////////////////////////////////////////////////////

void GlitchNoise::Debug_Gui() {
	if (ImGui::CollapsingHeader("GlitchNoise")) {
		ImGui::DragFloat("noiseTime", &noiseTime_, 0.01f);
		ImGui::DragFloat("strength ", &setting_->strength);
		ImGui::SliderFloat("currentTimer", &setting_->time, 0.0f, noiseTime_);
		saveSettings_.Debug_Gui();
		
		if (ImGui::Button("AddTime")) {
			setting_->time = 0.0f;
			setting_->frameIndex = 0;
		}
	}
}

void GlitchNoise::SaveSettings::Debug_Gui() {
	ImGui::DragFloat2("texelSize", &texelSize.x, 1);
}