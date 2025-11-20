#pragma once
#include <stack>
#include <string>
#include "Engine/DirectX/Utilities/DirectXUtils.h"

class DescriptorHeap;

enum DescriptorType {
	ShaderView,
	RTV,
	DSV,
};

/// <summary>
/// DescriptorのPoolを行うクラス
/// </summary>
class DescriptorAllocator {
public:

	DescriptorAllocator(DescriptorType _type, uint32_t _totalDescriptors, uint32_t _descriptorSize, int _index)
		: type_(_type), totalDescriptors_(_totalDescriptors), descriptorSize_(_descriptorSize), currentIndex_(_index) {
	}
	~DescriptorAllocator();

public:

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="descriptorHeap">: Heap</param>
	/// <returns></returns>
	DescriptorHandles Allocate(ID3D12DescriptorHeap* _descriptorHeap);

	/// <summary>
	/// 解放
	/// </summary>
	/// <param name="index">: heapのindex</param>
	void Free(uint32_t _index);

public:

	DescriptorHandles GetDescriptorHandle(ID3D12DescriptorHeap* _descriptorHeap, uint32_t _index);

private:

	DescriptorType type_;

	uint32_t totalDescriptors_;			// ディスクリプタの総数
	uint32_t descriptorSize_;			// 各ディスクリプタのサイズ
	int currentIndex_;					// 新規割り当て用のインデックス
	std::stack<uint32_t> freeStack_;	// 再利用可能なインデックスを保持するスタック

};

