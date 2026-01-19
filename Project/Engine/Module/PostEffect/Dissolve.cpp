#include "Dissolve.h"
#include "Engine.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/System/Manager/TextureManager.h"

using namespace AOENGINE;
using namespace PostEffect;

Dissolve::~Dissolve() {
	settingBuffer_->Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Dissolve::Init() {
	AOENGINE::GraphicsContext* graphicsCtx = AOENGINE::GraphicsContext::GetInstance();
	settingBuffer_ = graphicsCtx->CreateDxResource(ResourceType::Common);
	settingBuffer_->CreateResource(sizeof(DissolveSetting));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	setting_->uvTransform = Math::Matrix4x4::MakeUnit();
	setting_->color = Colors::Linear::white;
	setting_->edgeColor = Colors::Linear::white;
	setting_->threshold = 0.5f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void Dissolve::SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) {
	ApplySaveSettings();
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_Dissolve.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gSetting");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gMaskTexture");
	AOENGINE::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, "noise0.png", index);
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ チェックボックスの表示
///////////////////////////////////////////////////////////////////////////////////////////////

void Dissolve::CheckBox() {
	ImGui::Checkbox("Dissolve##Dissolve_checkbox", &isEnable_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存項目の適応
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::Dissolve::ApplySaveSettings() {
	setting_->uvTransform = saveSettings_.uvTransform.MakeAffine();
	setting_->color = saveSettings_.color;
	setting_->edgeColor = saveSettings_.edgeColor;
	setting_->threshold = saveSettings_.threshold;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::Dissolve::Save(const std::string& rootField) {
	saveSettings_.isEnable = isEnable_;
	saveSettings_.SetRootField(rootField);
	saveSettings_.Save();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::Dissolve::Load(const std::string& rootField) {
	saveSettings_.SetRootField(rootField);
	saveSettings_.Load();
	isEnable_ = saveSettings_.isEnable;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Dissolve::Debug_Gui() {
	if (ImGui::CollapsingHeader("Dissolve##Dissolve_Header")) {
		saveSettings_.Debug_Gui();
		saveSettings_.threshold = std::clamp(saveSettings_.threshold, 0.0f, 1.0f);
	}
}

void PostEffect::Dissolve::SaveDissolveSetting::Debug_Gui() {
	ImGui::DragFloat3("uvScale", &uvTransform.scale.x, 0.1f);
	ImGui::DragFloat3("uvRotate", &uvTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("uvTranslate", &uvTransform.translate.x, 0.1f);
	ImGui::ColorEdit4("color", &color.r);
	ImGui::ColorEdit4("edgeColor", &edgeColor.r);
	ImGui::DragFloat("threshold", &threshold, 0.1f);
}
