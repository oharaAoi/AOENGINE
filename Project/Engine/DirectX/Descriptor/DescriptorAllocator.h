#pragma once
#include <stack>
#include <string>
#include "Engine/DirectX/Utilities/DirectXUtils.h"

class DescriptorHeap;

enum DescriptorType {
	SAHADERVIEW,
	RTV,
	DSV,
};

class DescriptorAllocator {
public:

	DescriptorAllocator(DescriptorType type, uint32_t totalDescriptors, uint32_t descriptorSize, int index)
		: type_(type), totalDescriptors_(totalDescriptors), descriptorSize_(descriptorSize), currentIndex_(index) {
	}
	~DescriptorAllocator();

	DescriptorHandles Allocate(ID3D12DescriptorHeap* descriptorHeap);

	void Free(uint32_t index);

	DescriptorHandles GetDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap,
														  uint32_t index);

private:

	DescriptorType type_;

	uint32_t totalDescriptors_;			// ディスクリプタの総数
	uint32_t descriptorSize_;			// 各ディスクリプタのサイズ
	int currentIndex_;					// 新規割り当て用のインデックス
	std::stack<uint32_t> freeStack_;	// 再利用可能なインデックスを保持するスタック

};

