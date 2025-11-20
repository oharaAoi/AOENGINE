#include "DirectXCommands.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void DirectXCommands::Init(ID3D12Device* _device) {
	assert(_device);
	device_ = _device;

	CreateCommand();
	CreateFence();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void DirectXCommands::Finalize() {
	CloseHandle(fenceEvent_);
	fence_.Reset();

	commandQueue_.Reset();
	for (auto oi = 0; oi < kFrameCount_; ++oi) {
		commandAllocators_[oi].Reset();
	}
	commandList_.Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ DirectXのコマンドを生成
///////////////////////////////////////////////////////////////////////////////////////////////

void DirectXCommands::CreateCommand() {
	HRESULT hr = S_FALSE;
	// graphics用のコマンド系の初期化 --------------------------
	// GPUに命令を投げてくれる人　
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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Fenceの作成
///////////////////////////////////////////////////////////////////////////////////////////////

void DirectXCommands::CreateFence() {
	for (auto oi = 0; oi < kFrameCount_; ++oi) {
		fanceCounter_[oi] = 0;
	}

	// graphics用のフェンスの初期化 --------------------------
	HRESULT hr = S_FALSE;
	hr = device_->CreateFence(fanceCounter_[fenceIndex_], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	fanceCounter_[fenceIndex_]++;

	// Fenceのsignalを待つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, false, false, NULL);
	assert(fenceEvent_ != nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ GPUと同期を取る
///////////////////////////////////////////////////////////////////////////////////////////////

void DirectXCommands::SyncGPUAndCPU(uint32_t _currentIndex){
	// -----------------------------------------------
	// 下記の用にするとスレッドが大量終了する
	// -----------------------------------------------

	// 現在のバックバッファの Index を取得する（この時点では fenceIndex_ は古いフレームのまま）
	const auto prevFenceIndex = fenceIndex_;
	fenceIndex_ = _currentIndex;

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
