#pragma once
// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include "Engine/DirectX/Utilities/DirectXUtils.h"

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr <T>;

class DirectXDevice {
public:

	DirectXDevice() = default;
	~DirectXDevice() = default;

	void Init(IDXGIAdapter4* useAdapter);

	void Finalize();

public:

	ID3D12Device* GetDevice() { return device_.Get(); }

private:

	ComPtr<ID3D12Device> device_ = nullptr;

};