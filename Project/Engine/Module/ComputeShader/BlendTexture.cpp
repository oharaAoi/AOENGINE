#include "BlendTexture.h"

BlendTexture::BlendTexture() {
}

BlendTexture::~BlendTexture() {
	resultResource_->Finalize();
}

void BlendTexture::Init(ID3D12Device* device, DescriptorHeap* dxHeap) {
	D3D12_RESOURCE_DESC desc{};
	desc.Width = kWindowWidth_;			// 画面の横幅
	desc.Height = kWindowHeight_;			// 画面の縦幅
	desc.MipLevels = 1;			// 
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	// HEAPの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// resourceの作成
	resultResource_ = std::make_unique<DxResource>();
	resultResource_->Init(device, dxHeap, ResourceType::COMMON);
	resultResource_->CreateResource(&desc, &heapProperties, D3D12_HEAP_FLAG_ALLOW_DISPLAY, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	
	// ------------------------------------------------------------
	// UAVの設定
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resultResource_->CreateUAV(uavDesc);

	// ------------------------------------------------------------
	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	resultResource_->CreateSRV(srvDesc);

	groupCountX_ = (kWindowWidth_ + 16 - 1) / 16;
	groupCountY_ = (kWindowHeight_ + 16 - 1) / 16;

}

void BlendTexture::Execute(Pipeline* _pipeline, ID3D12GraphicsCommandList* commandList, const D3D12_GPU_DESCRIPTOR_HANDLE& refarebceGpuHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& rendrerGpuHandle) {
	UINT index = 0;
	index = _pipeline->GetRootSignatureIndex("gTexture");
	commandList->SetComputeRootDescriptorTable(index, refarebceGpuHandle);
	index = _pipeline->GetRootSignatureIndex("outputBuffer");
	commandList->SetComputeRootDescriptorTable(index, rendrerGpuHandle);
	commandList->Dispatch(groupCountX_, groupCountY_, 1);
}
