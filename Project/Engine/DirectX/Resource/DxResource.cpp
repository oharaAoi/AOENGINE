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

void DxResource::Init(ID3D12Device* _device, DescriptorHeap* _dxHeap, ResourceType _type) {

	assert(_device);
	assert(_dxHeap);

	pDevice_ = _device;
	pDxHeap_ = _dxHeap;

	type_ = _type;

	isDestroy_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　BufferResourceの作成
//////////////////////////////////////////////////////////////////////////////////////////////////

void DxResource::CreateResource(const size_t& _size) {
	cBuffer_ = CreateBufferResource(pDevice_, _size);
}

void DxResource::CreateResource(const D3D12_RESOURCE_DESC* _resourceDesc, const D3D12_HEAP_PROPERTIES* _heapProperties,
									const D3D12_HEAP_FLAGS& _heapFlags, const D3D12_RESOURCE_STATES& _resourceState) {
	desc_ = *_resourceDesc;
	HRESULT hr;
	hr = pDevice_->CreateCommittedResource(
		_heapProperties,
		_heapFlags,
		_resourceDesc,
		_resourceState,
		nullptr,
		IID_PPV_ARGS(&cBuffer_)
	);
	assert(SUCCEEDED(hr));

	bufferState_ = _resourceState;
}

void DxResource::CreateDepthResource(uint32_t _width, uint32_t _height) {
	cBuffer_ = CreateDepthStencilTextureResource(pDevice_, _width, _height);
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

void DxResource::CreateSRV(const D3D12_SHADER_RESOURCE_VIEW_DESC& _desc) {
	srvAddress_ = pDxHeap_->AllocateSRV();
	pDevice_->CreateShaderResourceView(cBuffer_.Get(), &_desc, srvAddress_.value().handleCPU);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓ UAVの生成
//////////////////////////////////////////////////////////////////////////////////////////////////

void DxResource::CreateUAV(const D3D12_UNORDERED_ACCESS_VIEW_DESC& _desc) {
	uavAddress_ = pDxHeap_->AllocateSRV();
	pDevice_->CreateUnorderedAccessView(cBuffer_.Get(), nullptr, &_desc, uavAddress_.value().handleCPU);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　RTVの生成
//////////////////////////////////////////////////////////////////////////////////////////////////

void DxResource::CreateRTV(const D3D12_RENDER_TARGET_VIEW_DESC& _desc) {
	rtvAddress_ = pDxHeap_->AllocateRTV();
	pDevice_->CreateRenderTargetView(cBuffer_.Get(), &_desc, rtvAddress_.value().handleCPU);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　swapChainのBufferにResourceを設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void DxResource::SetSwapChainBuffer(IDXGISwapChain4* _swapChain, uint32_t _index) {
	HRESULT hr = S_FALSE;
	hr = _swapChain->GetBuffer(_index, IID_PPV_ARGS(&cBuffer_));
	assert(SUCCEEDED(hr));
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　遷移させる
//////////////////////////////////////////////////////////////////////////////////////////////////

void DxResource::Transition(ID3D12GraphicsCommandList* _commandList, const D3D12_RESOURCE_STATES& _befor, const D3D12_RESOURCE_STATES& _after) {
	if (_befor != bufferState_) {
		Logger::Log("now : " + ResourceStateToString(bufferState_) + "\n");
		Logger::Log("target : " + ResourceStateToString(_befor) + "\n");
		Logger::Log("ResourceState MissMatch\n");
		assert("ResourceState MissMatch");
	}
	TransitionResourceState(_commandList, cBuffer_.Get(), _befor, _after);
	bufferState_ = _after;
}

void DxResource::Transition(ID3D12GraphicsCommandList* _commandList, const D3D12_RESOURCE_STATES& _after) {
	if (bufferState_ != _after) {
		TransitionResourceState(_commandList, cBuffer_.Get(), bufferState_, _after);
		bufferState_ = _after;
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
