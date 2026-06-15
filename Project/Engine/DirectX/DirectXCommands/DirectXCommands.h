#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <wrl.h>
// utilities
#include "Engine/DirectX/Utilities/DirectXUtils.h"
// DirectX系
#include "Engine/DirectX/DirectXDevice/DirectXDevice.h"

namespace AOENGINE {

/// <summary>
/// DirectX関連のコマンド
/// </summary>
class DirectXCommands {
public:

	DirectXCommands() = default;
	~DirectXCommands() = default;

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="device"></param>
	void Init(ID3D12Device* _device);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// DirectX12のコマンドを生成する
	/// </summary>
	void CreateCommand();

	/// <summary>
	/// Fenceを作成する
	/// </summary>
	void CreateFence();

	/// <summary>
	/// CPUとGPUの同期をはかる
	/// </summary>
	void SyncGPUAndCPU(uint32_t _currentIndex);


public:
	/// <summary>
	/// commandListをGPUに投げて実行させるもの
	/// </summary>
	/// <returns></returns>
	ID3D12CommandQueue* GetCommandQueue() { return commandQueue_.Get(); }

	ID3D12CommandAllocator* GetCommandAllocator() { return commandAllocators_[fenceIndex_].Get(); }

	ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }

private:

	static const uint32_t kFrameCount_ = 2;

	ID3D12Device* device_ = nullptr;

	// graphics用のコマンド系 ---------------------------------------------
	ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	ComPtr<ID3D12CommandAllocator> commandAllocators_[kFrameCount_];
	ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	// Fence & Event
	ComPtr<ID3D12Fence> fence_ = nullptr;
	HANDLE fenceEvent_;
	uint64_t fenceCounter_[kFrameCount_];
	uint32_t fenceIndex_;

};
}