 #include "DescriptorHeap.h"
#include "Engine/DirectX/Descriptor/DescriptorAllocator.h"
#include "Engine/DirectX/RTV/RenderTarget.h"

std::list<int> DescriptorHeap::freeSrvList_;
std::list<int> DescriptorHeap::freeRtvList_;

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void DescriptorHeap::Init(ID3D12Device* device) {
	assert(device);
	device_ = device;

	// サイズに定義
	if (!descriptorSize_) {
		descriptorSize_ = std::make_unique<DescriptorSize>(
			device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
			device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
		);
	}

	// rtvの数
	UINT rtvNum = RenderTargetType::kMAX;
	UINT swapChainNum = 2;
	UINT postProcessNum = 2;
	UINT particleEditorNum = 1;
	UINT sceneNum = 1;
	UINT imguiNum = 1;
	UINT totalRTVNum = rtvNum + swapChainNum + postProcessNum + particleEditorNum + sceneNum + imguiNum;

	// ヒープの生成
	rtvHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, totalRTVNum, false);
	srvHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, (1 << 16), true);
	dsvHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 4 + 1, false);

	// アロケータの初期化
	srvAllocator_ = std::make_unique<DescriptorAllocator>(
		DescriptorType::SAHADERVIEW,
		(1 << 16),
		descriptorSize_->GetSRV(),
		1
	);

	dsvAllocator_ = std::make_unique<DescriptorAllocator>(
		DescriptorType::DSV,
		5,
		descriptorSize_->GetDSV(),
		1
	);

	rtvAllocator_ = std::make_unique<DescriptorAllocator>(
		DescriptorType::RTV,
		totalRTVNum,
		descriptorSize_->GetRTV(),
		1
	);

	useSrvIndex_ = 0;	// SRVの先頭はImGuiで使うため0にして先頭を開けておく
	useDsvIndex_ = -1;	// 他は先頭から始められるように-1にしておくことで
	useRtvIndex_ = -1;	// GetDescriptorHandle時に先頭が0になる
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void DescriptorHeap::Finalize() {
	FreeList();
	rtvHeap_.Reset();
	srvHeap_.Reset();
	dsvHeap_.Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ accessor
///////////////////////////////////////////////////////////////////////////////////////////////

void DescriptorHeap::SetSRVHeap(ID3D12GraphicsCommandList* commandList) {
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvHeap_.Get() };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

DescriptorHandles DescriptorHeap::GetDescriptorHandle(const DescriptorHeapType& type) {
	DescriptorHandles handle{};

	switch (type) {
	case DescriptorHeapType::TYPE_SRV:
		handle.handleCPU = GetCPUDescriptorHandle(srvHeap_.Get(), descriptorSize_->GetSRV(), (static_cast<int>(useSrvIndex_) + 1));
		handle.handleGPU = GetGPUDescriptorHandle(srvHeap_.Get(), descriptorSize_->GetSRV(), (static_cast<int>(useSrvIndex_) + 1));
		useSrvIndex_++;
		handle.assignIndex_ = useSrvIndex_;
		break;

	case DescriptorHeapType::TYPE_DSV:
		handle.handleCPU = GetCPUDescriptorHandle(dsvHeap_.Get(), descriptorSize_->GetDSV(), (static_cast<int>(useDsvIndex_) + 1));
		useDsvIndex_++;
		handle.assignIndex_ = useDsvIndex_;
		break;

	case DescriptorHeapType::TYPE_RTV:
		handle.handleCPU = GetCPUDescriptorHandle(rtvHeap_.Get(), descriptorSize_->GetRTV(), (static_cast<int>(useRtvIndex_) + 1));
		useRtvIndex_++;
		handle.assignIndex_ = useRtvIndex_;
		break;
	}

	return handle;
}

void DescriptorHeap::FreeList() {
	for (int index : freeSrvList_) {
		FreeSRV(index);
	}
	for (int index : freeRtvList_) {
		FreeRTV(index);
	}
	freeSrvList_.clear();
	freeRtvList_.clear();
}

void DescriptorHeap::AddFreeSrvList(int index) {
	if (index >= 0) {
		freeSrvList_.push_back(index);
	}
}

void DescriptorHeap::AddFreeRtvList(int index) {
	if (index >= 0) {
		freeRtvList_.push_back(index);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 生成処理
///////////////////////////////////////////////////////////////////////////////////////////////

DescriptorHandles DescriptorHeap::AllocateSRV() {
 	return srvAllocator_->Allocate(srvHeap_.Get());
}

DescriptorHandles DescriptorHeap::AllocateRTV() {
	return rtvAllocator_->Allocate(rtvHeap_.Get());
}

DescriptorHandles DescriptorHeap::AllocateDSV() {
	return dsvAllocator_->Allocate(dsvHeap_.Get());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 解放処理
///////////////////////////////////////////////////////////////////////////////////////////////

void DescriptorHeap::FreeSRV(uint32_t index) {
	srvAllocator_->Free(index);
}

void DescriptorHeap::FreeRTV(uint32_t index) {
	rtvAllocator_->Free(index);
}

void DescriptorHeap::FreeDSV(uint32_t index) {
	dsvAllocator_->Free(index);
}
