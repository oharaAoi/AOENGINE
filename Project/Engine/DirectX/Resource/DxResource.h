#pragma once
#include <optional>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "dxgidebug.h"
#include <cassert>
#include "Engine/DirectX/Utilities/DirectXUtils.h"

enum class ResourceType {
	COMMON,
	RENDERTARGET,
	DEPTH
};

class DescriptorHeap;

/// <summary>
/// DirectXのResource
/// </summary>
class DxResource {
public: // コンストラクタ

	DxResource();
	~DxResource();

public:

	/// <summary>
	/// 解放要求
	/// </summary>
	void Destroy();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="device"></param>
	/// <param name="dxHeap"></param>
	/// <param name="type"></param>
	void Init(ID3D12Device* _device, DescriptorHeap* _dxHeap, ResourceType _type);

	/// <summary>
	/// リソース作成
	/// </summary>
	/// <param name="size"></param>
	void CreateResource(const size_t& _size);
	void CreateResource(const D3D12_RESOURCE_DESC* _resourceDesc, const D3D12_HEAP_PROPERTIES* _heapProperties,
							const D3D12_HEAP_FLAGS& _heapFlags, const D3D12_RESOURCE_STATES& _resourceState);
	void CreateDepthResource(uint32_t _width, uint32_t _height);

	void CreateCopyResource(ID3D12GraphicsCommandList* _commandList, DxResource* _source);

	/// <summary>
	/// Viewの作成
	/// </summary>
	/// <param name="desc"></param>
	void CreateSRV(const D3D12_SHADER_RESOURCE_VIEW_DESC& _desc);
	void CreateUAV(const D3D12_UNORDERED_ACCESS_VIEW_DESC& _desc);
	void CreateRTV(const D3D12_RENDER_TARGET_VIEW_DESC& _desc);

	/// <summary>
	/// Swapchainの設定
	/// </summary>
	/// <param name="swapChain"></param>
	/// <param name="index"></param>
	void SetSwapChainBuffer(IDXGISwapChain4* _swapChain, uint32_t index);

	/// <summary>
	/// 遷移
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="befor"></param>
	/// <param name="after"></param>
	void Transition(ID3D12GraphicsCommandList* _commandList, const D3D12_RESOURCE_STATES& _befor, const D3D12_RESOURCE_STATES& _after);
	void Transition(ID3D12GraphicsCommandList* _commandList, const D3D12_RESOURCE_STATES& _after);

public:

	ID3D12Resource* GetResource() { return cBuffer_.Get(); }
	ComPtr<ID3D12Resource> GetCompResource() { return cBuffer_; }

	const D3D12_RESOURCE_STATES& GetState() const { return bufferState_; }

	const DescriptorHandles& GetSRV() const;
	const DescriptorHandles& GetUAV() const;
	const DescriptorHandles& GetRTV() const;

	const D3D12_RESOURCE_DESC* GetDesc() const { return &desc_; }

	bool GetIsDestroy() const { return isDestroy_; }

private:

	ID3D12Device* pDevice_ = nullptr;
	DescriptorHeap* pDxHeap_ = nullptr;

	ComPtr<ID3D12Resource> cBuffer_;
	D3D12_RESOURCE_STATES bufferState_;

	std::optional<DescriptorHandles> uavAddress_ = std::nullopt;
	std::optional<DescriptorHandles> srvAddress_ = std::nullopt;
	std::optional<DescriptorHandles> rtvAddress_ = std::nullopt;

	D3D12_RESOURCE_DESC desc_;

	ResourceType type_;

	bool isDestroy_ = false;
};

