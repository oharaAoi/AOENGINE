#include "RenderTarget.h"
#include "Engine/WinApp/WinApp.h"

RenderTarget::RenderTarget() {
}

RenderTarget::~RenderTarget() {
	Finalize();
}

void RenderTarget::Finalize() {
	for (uint32_t oi = 0; oi < renderTargetNum_; ++oi) {
		if (renderTargetResource_[oi] != nullptr) {
			renderTargetResource_[oi]->Destroy();
		}
	}

	for (uint32_t oi = 0; oi < 2; ++oi) {
		if (swapChainResource_[oi] != nullptr) {
			swapChainResource_[oi]->Destroy();
		}
	}

	for (uint32_t oi = 0; oi < renderTargetNum_; ++oi) {
		renderTargetResource_[oi] = nullptr;
	}

	swapChainResource_[0] = nullptr;
	swapChainResource_[1] = nullptr;
}

void RenderTarget::Init(ID3D12Device* _device, AOENGINE::DescriptorHeap* _descriptorHeap, IDXGISwapChain4* _swapChain,
						ID3D12GraphicsCommandList* _commandList, DxResourceManager* _resourceManager) {
	assert(_descriptorHeap);
	assert(_swapChain);
	assert(_device);

	dxHeap_ = _descriptorHeap;
	swapChain_ = _swapChain;
	device_ = _device;
	resourceManager_ = _resourceManager;

	CrateSwapChainResource();
	CreateRenderTarget();

	ID3D12DescriptorHeap* descriptorHeaps[] = { dxHeap_->GetSRVHeap() };
	_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　RenderTargetを設定する
//////////////////////////////////////////////////////////////////////////////////////////////////
void RenderTarget::SetRenderTarget(ID3D12GraphicsCommandList* _commandList, const std::vector<RenderTargetType>& _renderTypes, const DescriptorHandles _dsvHandle) {
	// MRT用に複数のRTVハンドルを用意
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
	rtvHandles.reserve(_renderTypes.size());
	for (size_t index = 0; index < _renderTypes.size(); ++index) {
		rtvHandles.push_back(renderTargetResource_[_renderTypes[index]]->GetRTV().handleCPU);
	}

	_commandList->OMSetRenderTargets(static_cast<UINT>(rtvHandles.size()), rtvHandles.data(), FALSE, &_dsvHandle.handleCPU);
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	// RenderTargetはoffScreen用のRenderTargetを指定しておく
	// 各レンダーターゲットをクリア
	for (auto& rtv : rtvHandles) {
		_commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	}
	// 指定した深度で画面をクリア
	_commandList->ClearDepthStencilView(_dsvHandle.handleCPU, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	// srv
	ID3D12DescriptorHeap* descriptorHeaps[] = { dxHeap_->GetSRVHeap() };
	_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

void RenderTarget::ClearDepth(ID3D12GraphicsCommandList* _commandList) {
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxHeap_->GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();
	_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　swawChainで使用するRenderTargetを作成する
//////////////////////////////////////////////////////////////////////////////////////////////////

void RenderTarget::CrateSwapChainResource() {
	D3D12_RESOURCE_DESC desc{};
	desc.Width = WinApp::sWindowWidth;			// 画面の横幅
	desc.Height = WinApp::sWindowHeight;		// 画面の縦幅
	desc.MipLevels = 1;
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	// HEAPの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	for (uint32_t oi = 0; oi < 2; ++oi) {
		swapChainResource_[oi] = resourceManager_->CreateResource(ResourceType::RenderTarget);

		swapChainResource_[oi]->CreateResource(&desc, &heapProperties, D3D12_HEAP_FLAG_ALLOW_DISPLAY, D3D12_RESOURCE_STATE_RENDER_TARGET);
		swapChainResource_[oi]->SetSwapChainBuffer(swapChain_, oi);
		swapChainResource_[oi]->CreateRTV(rtvDesc);
		swapChainResource_[oi]->CreateSRV(srvDesc);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　swawChain以外のRenderTargetを作成する
//////////////////////////////////////////////////////////////////////////////////////////////////

void RenderTarget::CreateRenderTarget() {
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

	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	// 実際の初期化
	for (uint32_t oi = 0; oi < renderTargetNum_; ++oi) {
		if (oi == RenderTargetType::Sprite2d_RenderTarget) {
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		} else {
			rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		}
		renderTargetResource_[oi] = resourceManager_->CreateResource(ResourceType::RenderTarget);

		renderTargetResource_[oi]->CreateResource(&desc, &heapProperties, D3D12_HEAP_FLAG_ALLOW_DISPLAY, D3D12_RESOURCE_STATE_RENDER_TARGET);

		renderTargetResource_[oi]->CreateRTV(rtvDesc);
		renderTargetResource_[oi]->CreateSRV(srvDesc);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　RenderTargetの状態を遷移させる
//////////////////////////////////////////////////////////////////////////////////////////////////

void RenderTarget::TransitionResource(ID3D12GraphicsCommandList* _commandList, const RenderTargetType& _renderType, const D3D12_RESOURCE_STATES& _beforState, const D3D12_RESOURCE_STATES& _afterState) {
	renderTargetResource_[_renderType]->Transition(_commandList, _beforState, _afterState);
}

