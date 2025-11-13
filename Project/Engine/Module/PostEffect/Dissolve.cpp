#include "Dissolve.h"
#include "Engine.h"
#include "Engine/Core/GraphicsContext.h"

Dissolve::~Dissolve() {
	settingBuffer_->ReleaseRequest();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Dissolve::Init() {
	GraphicsContext* graphicsCtx = GraphicsContext::GetInstance();
	settingBuffer_ = std::make_unique<DxResource>();
	settingBuffer_->Init(graphicsCtx->GetDevice(), graphicsCtx->GetDxHeap(), ResourceType::COMMON);
	settingBuffer_->CreateResource(sizeof(DissolveSetting));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	setting_->uvTransform = Matrix4x4::MakeUnit();
	setting_->color = Color::white;
	setting_->edgeColor = Color::white;
	setting_->threshold = 0.5f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void Dissolve::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_Dissolve.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gSetting");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gMaskTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, "noise0.png", index);
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Dissolve::CheckBox() {
	ImGui::Checkbox("Dissolve##Dissolve_checkbox", &isEnable_);
}

void Dissolve::Debug_Gui() {
	if (ImGui::CollapsingHeader("Dissolve##Dissolve_Header")) {
		ImGui::DragFloat("threshold", &setting_->threshold, 0.01f);
		setting_->threshold = std::clamp(setting_->threshold, 0.0f, 1.0f);
	}
}