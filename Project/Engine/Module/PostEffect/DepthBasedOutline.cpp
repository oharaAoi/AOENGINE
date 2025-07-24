#include "DepthBasedOutline.h"
#include "Engine.h"
#include "Render.h"

DepthBasedOutline::~DepthBasedOutline() {
	settingBuffer_.reset();
}

void DepthBasedOutline::Init() {
	GraphicsContext* graphicsCtx = GraphicsContext::GetInstance();
	settingBuffer_ = std::make_unique<DxResource>();
	settingBuffer_->Init(graphicsCtx->GetDevice(), graphicsCtx->GetDxHeap(), ResourceType::COMMON);
	settingBuffer_->CreateResource(sizeof(Setting));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	depthResource_ = graphicsCtx->GetDxCommon()->GetDepthStencilResource();
	depthHandle_ = graphicsCtx->GetDxHeap()->AllocateSRV();

	D3D12_SHADER_RESOURCE_VIEW_DESC depthSrvDesc{};
	depthSrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	depthSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	depthSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	depthSrvDesc.Texture2D.MipLevels = 1;
	graphicsCtx->GetDevice()->CreateShaderResourceView(depthResource_, &depthSrvDesc, depthHandle_.handleCPU);

	setting_->edgeGain = 5.0f;
}

void DepthBasedOutline::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	setting_->projectionInverse = Render::GetProjection3D().Inverse();
	
	TransitionResourceState(commandList, depthResource_, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_DepthBasedOutline.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gDepthTexture");
	commandList->SetGraphicsRootDescriptorTable(index, depthHandle_.handleGPU);
	index = pso->GetRootSignatureIndex("gSetting");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);

	TransitionResourceState(commandList, depthResource_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,  D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void DepthBasedOutline::CheckBox() {
	ImGui::Checkbox("DepthBasedOutline##DepthBasedOutline_CheckBox", &isEnable_);
}

void DepthBasedOutline::Debug_Gui() {
	if (ImGui::CollapsingHeader("DepthBasedOutline##DepthBasedOutline_Header")) {
		ImGui::DragFloat("edgeGain", &setting_->edgeGain, 0.1f, 0.0f, 10.0f);
	}
}
