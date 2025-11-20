#pragma once
#include <memory>
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/DirectX/Resource/DxResourceManager.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"

enum class BufferType {
	Ping,
	Pong
};

/// <summary>
/// bufferを入れ替えてポストエフェクトを行う用のresource
/// </summary>
class PingPongBuffer {
public:

	PingPongBuffer() = default;
	~PingPongBuffer();

public:

	// 初期化
	void Init(ID3D12Device* _device, DescriptorHeap* _descriptorHeap, DxResourceManager* _dxResourceManager);
	// pingBufferの作成
	void CreatePing(DxResourceManager* _dxResourceManager);
	// pongBufferの作成
	void CreatePong(DxResourceManager* _dxResourceManager);
	// renderTargetの作成
	void SetRenderTarget(ID3D12GraphicsCommandList* commandList, BufferType type, const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle);
	// 遷移
	void Transition(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES after, BufferType type);
	// 入れ替え
	void Swap(ID3D12GraphicsCommandList* commandList);

public:		// accessor method

	DxResource* GetPingResource() { return pingResource_; }
	DxResource* GetPongResource() { return pongResource_; }

private:

	DxResource* pingResource_;	// pixelShaderで使用するtexture
	DxResource* pongResource_;	// renderTarget

	ID3D12Device* device_ = nullptr;
	DescriptorHeap* dxHeap_ = nullptr;

};

