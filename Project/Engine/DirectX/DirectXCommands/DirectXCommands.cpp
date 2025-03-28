#include "DirectXCommands.h"

DirectXCommands::DirectXCommands(ID3D12Device* device) {
	Initialize(device);
}

DirectXCommands::~DirectXCommands() {
}

void DirectXCommands::Initialize(ID3D12Device* device) {
	assert(device);
	device_ = device;

	CreateCommand();
	CreateFence();
}

void DirectXCommands::Finalize() {
	CloseHandle(fenceEvent_);
	fence_.Reset();

	commandQueue_.Reset();
	for (auto oi = 0; oi < kFrameCount_; ++oi) {
		commandAllocators_[oi].Reset();
	}
	commandList_.Reset();
}

/// <summary>
/// DirectX12のコマンドを生成する
/// </summary>
void DirectXCommands::CreateCommand() {
	HRESULT hr = S_FALSE;
	// graphics用のコマンド系の初期化 ======================================================================
	// GPUに命令を投げてくれる人　--------------------------
	// コマンドキューを生成する
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(hr));

	// CommandAllocatorの生成 --------------------------------
	// コマンドアロケータを生成する
	for (auto oi = 0; oi < kFrameCount_; ++oi) {
		hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators_[oi]));
		assert(SUCCEEDED(hr));
	}
	
	// コマンドリストを生成する ----------------------------
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators_[fenceIndex_].Get(), nullptr, IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(hr));
}

/// <summary>
/// Fenceを作成する
/// </summary>
void DirectXCommands::CreateFence() {
	for (auto oi = 0; oi < kFrameCount_; ++oi) {
		fanceCounter_[oi] = 0;
	}

	// graphics用のフェンスの初期化 ======================================================================
	HRESULT hr = S_FALSE;
	hr = device_->CreateFence(fanceCounter_[fenceIndex_], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	fanceCounter_[fenceIndex_]++;

	// Fenceのsignalを待つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, false, false, NULL);
	assert(fenceEvent_ != nullptr);
}

/// <summary>
/// CPUとGPUの同期をはかる
/// </summary>
void DirectXCommands::SyncGPUAndCPU(IDXGISwapChain4* swapChain){
	// 画面の交換を行う
	swapChain->Present(1, 0);

	// 現在のバックバッファの Index を取得する（この時点では fenceIndex_ は古いフレームのまま）
	const auto prevFenceIndex = fenceIndex_;
	fenceIndex_ = swapChain->GetCurrentBackBufferIndex();

	// 今フレームのフェンス値を取得
	const auto currentValue = fanceCounter_[prevFenceIndex];

	// コマンドキューにフェンスシグナルを送信
	commandQueue_->Signal(fence_.Get(), currentValue);

	// GPU の処理が完了していない場合は待機
	if (fence_->GetCompletedValue() < currentValue) {
		fence_->SetEventOnCompletion(currentValue, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	// **フェンスのカウンターを更新**
	fanceCounter_[fenceIndex_] = currentValue + 1;
}
