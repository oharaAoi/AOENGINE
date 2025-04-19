#include "PingPongBuffer.h"

PingPongBuffer::~PingPongBuffer() {
	pingResource_.reset();
	pongResource_.reset();
}

void PingPongBuffer::Init(ID3D12Device* device, DescriptorHeap* descriptorHeap) {
	device_ = device;
	dxHeap_ = descriptorHeap;

	CreatePing();
	CreatePong();
}

void PingPongBuffer::CreatePing() {
	// 最終的に描画させるResourceの作成
	D3D12_RESOURCE_DESC desc{};
	desc.Width = kWindowWidth_;			// 画面の横幅
	desc.Height = kWindowHeight_;		// 画面の縦幅
	desc.MipLevels = 1;			// 
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	// HEAPの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// ------------------------------------------------------------
	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	pingResource_ = std::make_unique<DxResource>();
	pingResource_->Init(device_, dxHeap_, ResourceType::COMMON);

	pingResource_->CreateResource(&desc, &heapProperties, D3D12_HEAP_FLAG_ALLOW_DISPLAY, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	pingResource_->CreateSRV(srvDesc);
}

void PingPongBuffer::CreatePong() {
	// resourceの設定
	D3D12_RESOURCE_DESC desc{};
	desc.Width = kWindowWidth_;			// 画面の横幅
	desc.Height = kWindowHeight_;			// 画面の縦幅
	desc.MipLevels = 1;			// 
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	// HEAPの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	pongResource_ = std::make_unique<DxResource>();
	pongResource_->Init(device_, dxHeap_, ResourceType::RENDERTARGET);

	pongResource_->CreateResource(&desc, &heapProperties, D3D12_HEAP_FLAG_ALLOW_DISPLAY, D3D12_RESOURCE_STATE_RENDER_TARGET);
	pongResource_->CreateRTV(rtvDesc);
}

void PingPongBuffer::SetRenderTarget(ID3D12GraphicsCommandList* commandList) {
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxHeap_->GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();
	
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles{};
	//rtvHandles = RTVHandle_[type].handleCPU;
	commandList->OMSetRenderTargets(1, &pongResource_->GetRTV().handleCPU, false, &dsvHandle);
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	// RenderTargetはoffScreen用のRenderTargetを指定しておく
	commandList->ClearRenderTargetView(pongResource_->GetRTV().handleCPU, clearColor, 0, nullptr);
	// 指定した深度で画面をクリア
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	// srv
	ID3D12DescriptorHeap* descriptorHeaps[] = { dxHeap_->GetSRVHeap() };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

void PingPongBuffer::Transition(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES after, BufferType type) {
	switch (type) {
	case BufferType::PING:
		pingResource_->Transition(commandList, after);
		break;
	case BufferType::PONG:
		pongResource_->Transition(commandList, after);
		break;
	default:
		break;
	}
}

void PingPongBuffer::Swap() {
	std::swap(pingResource_, pongResource_);
}
