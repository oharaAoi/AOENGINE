#include "ShadowMap.h"
#include "Engine/Render.h"
#include "Engine/Core/GraphicsContext.h"

ShadowMap::~ShadowMap() {
}

void ShadowMap::Init() {
	GraphicsContext* ctx = GraphicsContext::GetInstance();
	ID3D12Device* device = ctx->GetDevice();
	DescriptorHeap* descriptorHeap = ctx->GetDxHeap();

	depthStencilResource_ = std::make_unique<DxResource>();
	depthStencilResource_->Init(device, descriptorHeap, ResourceType::DEPTH);
	depthStencilResource_->CreateDepthResource(kWindowWidth_, kWindowHeight_);

	// heap上にDSCを構築
	D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	depthDsvHandle_ = descriptorHeap->AllocateDSV();
	device->CreateDepthStencilView(depthStencilResource_->GetResource(), &desc, depthDsvHandle_.handleCPU);

	depthSrvHandle_ = descriptorHeap->AllocateSRV();
	D3D12_SHADER_RESOURCE_VIEW_DESC depthSrvDesc{};
	depthSrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	depthSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	depthSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	depthSrvDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(depthStencilResource_->GetResource(), &depthSrvDesc, depthSrvHandle_.handleCPU);
}

void ShadowMap::SetCommand() {
	std::vector<RenderTargetType> types;
	types.push_back(RenderTargetType::ShadowMap_RenderTarget);
	Render::SetRenderTarget(types, depthDsvHandle_);
}

void ShadowMap::ChangeResource(ID3D12GraphicsCommandList* commandList) {
	TransitionResourceState(commandList, depthStencilResource_->GetResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void ShadowMap::ResetResource(ID3D12GraphicsCommandList* commandList) {
	TransitionResourceState(commandList, depthStencilResource_->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void ShadowMap::Debug_Gui() {
	ImTextureID textureID = reinterpret_cast<ImTextureID>(static_cast<uint64_t>(depthSrvHandle_.handleGPU.ptr));
	ImGui::SetCursorPos(ImVec2(20, 60)); // 描画位置を設定
	ImVec2 availSize = ImGui::GetContentRegionAvail();
	// アスペクト比維持したい場合はここで調整
	float aspect = 16.0f / 9.0f;
	if (availSize.x / availSize.y > aspect) {
		// 横が余る → 高さ基準に合わせる
		availSize.x = availSize.y * aspect;
	} else {
		// 縦が余る → 横基準に合わせる
		availSize.y = availSize.x / aspect;
	}
	ImGui::Image(textureID, availSize, ImVec2(0, 0), ImVec2(1, 1));
}
