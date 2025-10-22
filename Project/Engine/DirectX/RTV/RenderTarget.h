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

/// <summary>
/// RenderTargetクラス
/// </summary>
class RenderTarget {
public: // コンストラクタ

	RenderTarget();
	~RenderTarget();

public: 

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="device">: デバイス</param>
	/// <param name="descriptorHeap">: ヒープ</param>
	/// <param name="swapChain">: スワップチェイン</param>
	/// <param name="commandList">: コマンドリスト</param>
	void Init(ID3D12Device* device, DescriptorHeap* descriptorHeap, IDXGISwapChain4* swapChain, ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// RenderTargetを設定する
	/// </summary>
	/// <param name="commandList">: コマンドリスト</param>
	/// <param name="renderTypes">: レンダーターゲットのタイプ</param>
	/// <param name="dsvHandle">: デプスハンドル</param>
	void SetRenderTarget(ID3D12GraphicsCommandList* commandList, const std::vector<RenderTargetType>& renderTypes, const DescriptorHandles dsvHandle);

	/// <summary>
	/// 深度バッファ
	/// </summary>
	/// <param name="commandList"></param>
	void ClearDepth(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// swawChainで使用するRenderTargetを作成する
	/// </summary>
	void CrateSwapChainResource();

	/// <summary>
	/// swawChain以外のRenderTargetを作成する
	/// </summary>
	void CreateRenderTarget();

	/// <summary>
	/// RenderTargetを遷移させる
	/// </summary>
	/// <param name="commandList">: commandList</param>
	/// <param name="renderType">: RenderTargetのタイプ</param>
	/// <param name="beforState">: 遷移前の状態</param>
	/// <param name="afterState">: 遷移後の状態</param>
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