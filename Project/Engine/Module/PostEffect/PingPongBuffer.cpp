#include "PingPongBuffer.h"
#include "Engine/WinApp/WinApp.h"

PingPongBuffer::~PingPongBuffer() {
	pingResource_->Destroy();
	pongResource_->Destroy();
}

void PingPongBuffer::Init(ID3D12Device* _device, DescriptorHeap* _descriptorHeap, DxResourceManager* _dxResourceManager) {
	device_ = _device;
	dxHeap_ = _descriptorHeap;

	CreatePing(_dxResourceManager);
	CreatePong(_dxResourceManager);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ pingバッファを作成
///////////////////////////////////////////////////////////////////////////////////////////////

void PingPongBuffer::CreatePing(DxResourceManager* _dxResourceManager) {
	// 最終的に描画させるResourceの作成
	// resourceの設定
	D3D12_RESOURCE_DESC desc{};
	desc.Width = WinApp::sWindowWidth;			// 画面の横幅
	desc.Height = WinApp::sWindowHeight;		// 画面の縦幅
	desc.MipLevels = 1;
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	// HEAPの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	pingResource_ = _dxResourceManager->CreateResource(ResourceType::Common);
	pingResource_->CreateResource(&desc, &heapProperties, D3D12_HEAP_FLAG_ALLOW_DISPLAY, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	pingResource_->CreateRTV(rtvDesc);
	pingResource_->CreateSRV(srvDesc);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ pongバッファを作成
///////////////////////////////////////////////////////////////////////////////////////////////

void PingPongBuffer::CreatePong(DxResourceManager* _dxResourceManager) {
	// resourceの設定
	D3D12_RESOURCE_DESC desc{};
	desc.Width = WinApp::sWindowWidth;			// 画面の横幅
	desc.Height = WinApp::sWindowHeight;		// 画面の縦幅
	desc.MipLevels = 1;
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	// HEAPの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	pongResource_ = _dxResourceManager->CreateResource(ResourceType::Common);
	pongResource_->CreateResource(&desc, &heapProperties, D3D12_HEAP_FLAG_ALLOW_DISPLAY, D3D12_RESOURCE_STATE_RENDER_TARGET);
	pongResource_->CreateRTV(rtvDesc);
	pongResource_->CreateSRV(srvDesc);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ RenderTargetを設定
///////////////////////////////////////////////////////////////////////////////////////////////

void PingPongBuffer::SetRenderTarget(ID3D12GraphicsCommandList* commandList, BufferType type, const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle) {
	DxResource* resource = nullptr;
	switch (type) {
	case BufferType::Ping:
		resource = pingResource_;
		break;
	case BufferType::Pong:
		resource = pongResource_;
		break;
	default:
		break;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles{};
	//rtvHandles = RTVHandle_[type].handleCPU;
	commandList->OMSetRenderTargets(1, &resource->GetRTV().handleCPU, false, &dsvHandle);
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	// RenderTargetはoffScreen用のRenderTargetを指定しておく
	commandList->ClearRenderTargetView(resource->GetRTV().handleCPU, clearColor, 0, nullptr);
	// 指定した深度で画面をクリア
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	// srv
	ID3D12DescriptorHeap* descriptorHeaps[] = { dxHeap_->GetSRVHeap() };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 遷移
///////////////////////////////////////////////////////////////////////////////////////////////

void PingPongBuffer::Transition(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES after, BufferType type) {
	switch (type) {
	case BufferType::Ping:
		pingResource_->Transition(commandList, after);
		break;
	case BufferType::Pong:
		pongResource_->Transition(commandList, after);
		break;
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Resourceを入れ替える
///////////////////////////////////////////////////////////////////////////////////////////////

void PingPongBuffer::Swap(ID3D12GraphicsCommandList* commandList) {
	std::swap(pingResource_, pongResource_);
	pingResource_->Transition(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	pongResource_->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
	
}
