#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <wrl.h>
#include <memory>
// winapp
#include "Engine/WinApp/WinApp.h"
// DirectX
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/DirectX/Resource/DxResourceManager.h"
#include "Engine/DirectX/DirectXCommands/DirectXCommands.h"
// DXC
#include <dxcapi.h>
// RT
#include "Engine/DirectX/RTV/RenderTarget.h"
// utilities
#include "Engine/Utilities/Convert.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Utilities/Debug.h"

namespace AOENGINE {

/// <summary>
/// 初期化やFrameの開始に必要な変数を持ったクラス
/// </summary>
class DirectXCommon {
public: // コンストラクタ

	DirectXCommon() = default;
	~DirectXCommon() = default;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(WinApp* _win, int32_t _backBufferWidth, int32_t _backBufferHeight);

	/// <summary>
	/// 色々な設定をする
	/// </summary>
	void Setting(ID3D12Device* _device, DirectXCommands* _dxCommands, AOENGINE::DescriptorHeap* _descriptorHeaps, RenderTarget* _renderTarget, AOENGINE::DxResourceManager* _resourceManager);

	/// <summary>
	/// 終了関数
	/// </summary>
	void Finalize();

	/// <summary>
	/// フレームを開始する
	/// </summary>
	void Begin();

	/// <summary>
	/// フレームを終了する
	/// </summary>
	void End();

	void SetSwapChain();

public:

	/// <summary>
	/// DXGIFactoryの生成
	/// </summary>
	void CreateDXGI();

	/// <summary>
	/// 使用するGUPを決める
	/// </summary>
	void SetUseGPU();

	/// <summary>
	/// エラーの設定
	/// </summary>
	void SetError();

	/// <summary>
	/// swapchainを生成する
	/// </summary>
	/// <param name="commandQueue"></param>
	void CreateSwapChain();

	/// <summary>
	/// Fenceの生成
	/// </summary>
	void CrateFence();

	/// <summary>
	/// Viewport変換
	/// </summary>
	void SetViewport();

	/// <summary>
	/// DSVの設定をする
	/// </summary>
	void CreateDSV();

	////////////////////////////////////////////////////////////////////////////////////
	// accessor
	////////////////////////////////////////////////////////////////////////////////////

	int32_t GetSwapChainBfCount() { return swapChainBufferCount_; }

	D3D12_GPU_DESCRIPTOR_HANDLE GetBackBufferGpuHandle();

public:

	IDXGIAdapter4* GetUseAdapter() { return useAdapter_.Get(); }

	ComPtr<IDXGISwapChain4> GetSwapChain() { return swapChain_.Get(); }

	ID3D12Resource* GetDepthStencilResource() { return depthStencilResource_->GetResource(); }

	const DescriptorHandles& GetDepthHandle() const { return depthHandle_; }

private:

	int32_t kClientWidth_;
	int32_t kClientHeight_;

	int32_t swapChainBufferCount_;

	WinApp* winApp_ = nullptr;
	AOENGINE::DescriptorHeap* descriptorHeaps_ = nullptr;
	DirectXCommands* dxCommands_ = nullptr;
	ID3D12Device* device_ = nullptr;
	AOENGINE::DxResourceManager* dxResourceManager_ = nullptr;

	RenderTarget* renderTarget_ = nullptr;

private:

	// デバック =======================================================================================
	ComPtr<ID3D12Debug1> debugController_ = nullptr;

	// 初期化系 =======================================================================================
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;

	// 生成する変数 =======================================================================================
	// swapChaim
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	// Fence & Event
	ComPtr<ID3D12Fence> fence_ = nullptr;
	uint64_t fenceValue_;
	HANDLE fenceEvent_;

	// barrier
	D3D12_RESOURCE_BARRIER barrier_;

	D3D12_VIEWPORT viewport_;
	D3D12_RECT scissorRect_;

	// dsv
	AOENGINE::DxResource* depthStencilResource_ = nullptr;
	DescriptorHandles depthHandle_;
};

}