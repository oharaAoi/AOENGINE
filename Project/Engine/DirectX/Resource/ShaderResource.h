#pragma once
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"

/// <summary>
/// Shaderで使用するResource
/// </summary>
class ShaderResource {
public: // コンストラクタ

	ShaderResource();
	~ShaderResource();

public:

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="device"></param>
	/// <param name="dxHeap"></param>
	/// <param name="resourceDesc"></param>
	/// <param name="heapProperties"></param>
	/// <param name="heapFlags"></param>
	/// <param name="resourceState"></param>
	void Init(ID3D12Device* device, DescriptorHeap* dxHeap,
			  D3D12_RESOURCE_DESC resourceDesc, D3D12_HEAP_PROPERTIES* heapProperties,
			  const D3D12_HEAP_FLAGS& heapFlags, const D3D12_RESOURCE_STATES& resourceState);

	/// <summary>
	/// viewの作成
	/// </summary>
	/// <param name="desc"></param>
	void CreateSRV(const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);
	void CreateUAV(const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc);

	/// <summary>
	/// 遷移
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="befor"></param>
	/// <param name="after"></param>
	void Transition(ID3D12GraphicsCommandList* commandList, const D3D12_RESOURCE_STATES& befor, const D3D12_RESOURCE_STATES& after);
	void Transition(ID3D12GraphicsCommandList* commandList, const D3D12_RESOURCE_STATES& after);

public:

	ID3D12Resource* GetResource() { return cBuffer_.Get(); }

	const D3D12_RESOURCE_STATES& GetState() const { return bufferState_; }

	const DescriptorHandles& GetSRV() const { return srvAddress_; }
	const DescriptorHandles& GetUAV() const { return uavAddress_; }

private:

	ID3D12Device* pDevice_ = nullptr;
	DescriptorHeap* pDxHeap_ = nullptr;

	ComPtr<ID3D12Resource> cBuffer_;
	D3D12_RESOURCE_STATES bufferState_;

	DescriptorHandles uavAddress_;
	DescriptorHandles srvAddress_;
};

