#pragma once
#include <vector>
#include "Engine/Utilities/Convert.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
#include "Engine/DirectX/Descriptor/DescriptorSize.h"
#include "Engine/DirectX/Resource/DxResource.h"

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr <T>;

enum RenderTargetType {
	Object3D_RenderTarget,
	MotionVector_RenderTarget,
	OffScreen_RenderTarget,
	Sprite2d_RenderTarget,
	EffectSystem_RenderTarget,
	PreEffectSystem_RenderTarget,
	ShadowMap_RenderTarget,
	kMAX
};

static const uint32_t renderTargetNum_ = (RenderTargetType::kMAX);

class RenderTarget {
public:

	RenderTarget();
	~RenderTarget();

	void Finalize();

	void Init(ID3D12Device* device, DescriptorHeap* descriptorHeap, IDXGISwapChain4* swapChain, ID3D12GraphicsCommandList* commandList);

	void SetRenderTarget(ID3D12GraphicsCommandList* commandList, const std::vector<RenderTargetType>& renderTypes, const DescriptorHandles dsvHandle);

	void ClearDepth(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// swawChainで使用するRenderTargetを作成する
	/// </summary>
	void CrateSwapChainResource();

	/// <summary>
	/// swawChain以外のRenderTargetを作成する
	/// </summary>
	void CreateRenderTarget();

	void TransitionResource(ID3D12GraphicsCommandList* commandList, const RenderTargetType& renderType, const D3D12_RESOURCE_STATES& beforState, const D3D12_RESOURCE_STATES& afterState);

public:

	ID3D12Resource* GetSwapChainRenderResource(const UINT& indexNum) { return swapChainResource_[indexNum]->GetResource(); }

	const DescriptorHandles& GetSwapChainHandle(const UINT& indexNum) { return swapChainResource_[indexNum]->GetRTV(); }
	const DescriptorHandles& GetSwapChainSRVHandle(const UINT& indexNum) { return swapChainResource_[indexNum]->GetSRV(); }

	const DescriptorHandles& GetRenderTargetRTVHandle(const RenderTargetType& type) const { return renderTargetResource_[type]->GetRTV(); }
	const DescriptorHandles& GetRenderTargetSRVHandle(const RenderTargetType& type) const { return renderTargetResource_[type]->GetSRV(); }

	DxResource* GetRenderTargetResource(const RenderTargetType& type) { return renderTargetResource_[type].get(); }

private:

	std::unique_ptr<DxResource> swapChainResource_[2];
	std::unique_ptr<DxResource> renderTargetResource_[renderTargetNum_];

	ID3D12Device* device_ = nullptr;
	// heap
	DescriptorHeap* dxHeap_ = nullptr;
	// swapChain
	IDXGISwapChain4* swapChain_ = nullptr;
};