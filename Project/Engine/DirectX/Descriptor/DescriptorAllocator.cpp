#include "DescriptorAllocator.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
#include "Engine/Utilities/Logger.h"

using namespace AOENGINE;

DescriptorAllocator::~DescriptorAllocator() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 生成をする
///////////////////////////////////////////////////////////////////////////////////////////////

DescriptorHandles DescriptorAllocator::Allocate(ID3D12DescriptorHeap* _descriptorHeap) {
	if (!freeStack_.empty()) {
		// 再利用可能なインデックスをスタックから取得
		uint32_t reusedIndex = freeStack_.front();
		freeStack_.pop();
		return GetDescriptorHandle(_descriptorHeap, reusedIndex);
	} else if (currentIndex_ < (int)totalDescriptors_) {
		// 新しいディスクリプタを割り当て
		return GetDescriptorHandle(_descriptorHeap, currentIndex_++);
	} else {
		throw std::runtime_error("No more descriptors available.");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 解放をする
///////////////////////////////////////////////////////////////////////////////////////////////

void DescriptorAllocator::Free(uint32_t _index) {
	freeStack_.push(_index);  // フリースタックに戻す
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次のポインタハンドルを返す
///////////////////////////////////////////////////////////////////////////////////////////////

DescriptorHandles DescriptorAllocator::GetDescriptorHandle(ID3D12DescriptorHeap* _descriptorHeap, uint32_t _index) {
	DescriptorHandles handles;
	handles.handleCPU = _descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	if (type_ == ShaderView) {
		handles.handleGPU = _descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	} else if (type_ == DSV) {

	}
	handles.assignIndex_ = _index;
	// インデックスに基づきディスクリプタのオフセットを計算
	handles.handleCPU.ptr += descriptorSize_ * _index;
	if (type_ == ShaderView) {
		handles.handleGPU.ptr += descriptorSize_ * _index;
	}
	return handles;
}
