#include "DescriptorAllocator.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
#include "Engine/Utilities/Logger.h"

DescriptorAllocator::~DescriptorAllocator() {
}

DescriptorHandles DescriptorAllocator::Allocate(ID3D12DescriptorHeap* descriptorHeap) {
	if (!freeStack_.empty()) {
		// 再利用可能なインデックスをスタックから取得
		uint32_t reusedIndex = freeStack_.top();
		freeStack_.pop();
		/*std::string name = std::to_string(reusedIndex);
		Log("popHeap" + name + "\n");*/
		return GetDescriptorHandle(descriptorHeap, reusedIndex);
	} else if (currentIndex_ < (int)totalDescriptors_) {
		// 新しいディスクリプタを割り当て
		return GetDescriptorHandle(descriptorHeap, currentIndex_++);
	} else {
		throw std::runtime_error("No more descriptors available.");
	}
}

void DescriptorAllocator::Free(uint32_t index) {
	freeStack_.push(index);  // フリースタックに戻す
	std::string name = std::to_string(index);
	Logger::Log("pushFreeHeap" + name + "\n");
}

DescriptorHandles DescriptorAllocator::GetDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t index) {
	DescriptorHandles handles;
	handles.handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	if (type_ == SAHADERVIEW) {
		handles.handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	} else if (type_ == DSV) {

	}
	handles.assignIndex_ = index;
	// インデックスに基づきディスクリプタのオフセットを計算
	handles.handleCPU.ptr += descriptorSize_ * index;
	if (type_ == SAHADERVIEW) {
		handles.handleGPU.ptr += descriptorSize_ * index;
	}
	return handles;
}
