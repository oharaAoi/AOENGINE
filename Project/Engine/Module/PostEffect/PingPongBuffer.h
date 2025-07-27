#pragma once
#include <memory>
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"

enum class BufferType {
	PING,
	PONG
};

class PingPongBuffer {
public:

	PingPongBuffer() = default;
	~PingPongBuffer();

	void Init(ID3D12Device* device, DescriptorHeap* descriptorHeap);

	void CreatePing();

	void CreatePong();

	void SetRenderTarget(ID3D12GraphicsCommandList* commandList, BufferType type, const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle);

	void Transition(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES after, BufferType type);

	void Swap(ID3D12GraphicsCommandList* commandList);

public:		// accessor method

	DxResource* GetPingResource() { return pingResource_.get(); }
	DxResource* GetPongResource() { return pongResource_.get(); }

private:

	std::unique_ptr<DxResource> pingResource_;	// pixelShaderで使用するtexture
	std::unique_ptr<DxResource> pongResource_;	// renderTarget

	ID3D12Device* device_ = nullptr;
	DescriptorHeap* dxHeap_ = nullptr;

};

