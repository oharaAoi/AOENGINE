#include "DxResource.h"
#include <cassert>
#include "Engine/Utilities/Logger.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"

DxResource::DxResource() {}
DxResource::~DxResource() {
	if (cBuffer_ = nullptr) {
		return;
	}
	cBuffer_.Reset();
	if (srvAddress_ != std::nullopt) {
		DescriptorHeap::AddFreeSrvList(srvAddress_.value().assignIndex_);
	}
	if (uavAddress_ != std::nullopt) {
		DescriptorHeap::AddFreeSrvList(uavAddress_.value().assignIndex_);
	}
	if (rtvAddress_ != std::nullopt) {
		DescriptorHeap::AddFreeRtvList(rtvAddress_.value().assignIndex_);
	}
}

void DxResource::Destroy() {
	isDestroy_ = true;
}

void DxResource::Init(ID3D12Device* device, DescriptorHeap* dxHeap, ResourceType type) {

	assert(device);
	assert(dxHeap);

	pDevice_ = device;
	pDxHeap_ = dxHeap;

	type_ = type;

	isDestroy_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　BufferResourceの作成
//////////////////////////////////////////////////////////////////////////////////////////////////

void DxResource::CreateResource(const size_t& size) {
	cBuffer_ = CreateBufferResource(pDevice_, size);
}

void DxResource::CreateResource(const D3D12_RESOURCE_DESC* resourceDesc, const D3D12_HEAP_PROPERTIES* heapProperties,
									const D3D12_HEAP_FLAGS& heapFlags, const D3D12_RESOURCE_STATES& resourceState) {
	desc_ = *resourceDesc;
	HRESULT hr;
	hr = pDevice_->CreateCommittedResource(
		heapProperties,
		heapFlags,
		resourceDesc,
		resourceState,
		nullptr,
		IID_PPV_ARGS(&cBuffer_)
	);
	assert(SUCCEEDED(hr));

	bufferState_ = resourceState;
}

void DxResource::CreateDepthResource(uint32_t width, uint32_t height) {
	cBuffer_ = CreateDepthStencilTextureResource(pDevice_, width, height);
}

void DxResource::CreateCopyResource(ID3D12GraphicsCommandList* _commandList, DxResource* _source) {
	// ----------------------
	// ↓ resourceの作成
	// ----------------------
	// resourceの設定
	D3D12_RESOURCE_DESC desc{};
	desc.Width = WinApp::sWindowWidth;			// 画面の横幅
	desc.Height = WinApp::sWindowHeight;			// 画面の縦幅
	desc.MipLevels = 1;			// 
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	// HEAPの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	this->CreateResource(&desc, &heapProperties, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	CreateSRV(srvDesc);

	// ----------------------
	// ↓ 基のresourceをコピーする
	// ----------------------
	D3D12_RESOURCE_STATES sourceState = _source->GetState();
	_source->Transition(_commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
	_commandList->CopyResource(cBuffer_.Get(), _source->GetResource());
	_source->Transition(_commandList, sourceState);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　SRVの生成
//////////////////////////////////////////////////////////////////////////////////////////////////

void DxResource::CreateSRV(const D3D12_SHADER_RESOURCE_VIEW_DESC& desc) {
	srvAddress_ = pDxHeap_->AllocateSRV();
	pDevice_->CreateShaderResourceView(cBuffer_.Get(), &desc, srvAddress_.value().handleCPU);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓ UAVの生成
//////////////////////////////////////////////////////////////////////////////////////////////////

void DxResource::CreateUAV(const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc) {
	uavAddress_ = pDxHeap_->AllocateSRV();
	pDevice_->CreateUnorderedAccessView(cBuffer_.Get(), nullptr, &desc, uavAddress_.value().handleCPU);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　RTVの生成
//////////////////////////////////////////////////////////////////////////////////////////////////

void DxResource::CreateRTV(const D3D12_RENDER_TARGET_VIEW_DESC& desc) {
	rtvAddress_ = pDxHeap_->AllocateRTV();
	pDevice_->CreateRenderTargetView(cBuffer_.Get(), &desc, rtvAddress_.value().handleCPU);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　swapChainのBufferにResourceを設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void DxResource::SetSwapChainBuffer(IDXGISwapChain4* swapChain, uint32_t index) {
	HRESULT hr = S_FALSE;
	hr = swapChain->GetBuffer(index, IID_PPV_ARGS(&cBuffer_));
	assert(SUCCEEDED(hr));
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　遷移させる
//////////////////////////////////////////////////////////////////////////////////////////////////

void DxResource::Transition(ID3D12GraphicsCommandList* commandList, const D3D12_RESOURCE_STATES& befor, const D3D12_RESOURCE_STATES& after) {
	if (befor != bufferState_) {
		Logger::Log("now : " + ResourceStateToString(bufferState_) + "\n");
		Logger::Log("target : " + ResourceStateToString(befor) + "\n");
		Logger::Log("ResourceState MissMatch\n");
		assert("ResourceState MissMatch");
	}
	TransitionResourceState(commandList, cBuffer_.Get(), befor, after);
	bufferState_ = after;
}

void DxResource::Transition(ID3D12GraphicsCommandList* commandList, const D3D12_RESOURCE_STATES& after) {
	if (bufferState_ != after) {
		TransitionResourceState(commandList, cBuffer_.Get(), bufferState_, after);
		bufferState_ = after;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　ViewのGetter
//////////////////////////////////////////////////////////////////////////////////////////////////

const DescriptorHandles& DxResource::GetSRV() const {
	if (srvAddress_ == std::nullopt) {
		assert("not Setting SRV");
	}
	return srvAddress_.value();
}

const DescriptorHandles& DxResource::GetUAV() const {
	if (uavAddress_ == std::nullopt) {
		assert("not Setting UAV");
	}
	return uavAddress_.value();
}

const DescriptorHandles& DxResource::GetRTV() const {
	if (rtvAddress_ == std::nullopt) {
		assert("not Setting RTV");
	}
	return rtvAddress_.value();
}
