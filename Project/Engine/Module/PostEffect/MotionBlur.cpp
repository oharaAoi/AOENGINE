#include "MotionBlur.h"
#include "Engine.h"
#include "Engine/Module/PostEffect/PostProcess.h"

using namespace AOENGINE;
using namespace PostEffect;

MotionBlur::~MotionBlur() {
	motionResource_->Destroy();
}

void MotionBlur::Init() {
}

void PostEffect::MotionBlur::PostInit(AOENGINE::PostProcess* _owner) {
	SetMotionResource(_owner->GetMotionBluerRnderTarget());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void MotionBlur::SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) {
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


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::MotionBlur::Save(const std::string& rootField) {
	saveSettings_.isEnable = isEnable_;
	saveSettings_.SetRootField(rootField);
	saveSettings_.Save();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::MotionBlur::Load(const std::string& rootField) {
	saveSettings_.SetRootField(rootField);
	saveSettings_.Load();
	isEnable_ = saveSettings_.isEnable;
}
