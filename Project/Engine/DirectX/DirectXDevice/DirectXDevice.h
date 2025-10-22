#pragma once
// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include "Engine/DirectX/Utilities/DirectXUtils.h"

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr <T>;

/// <summary>
/// DirectXDeviceのクラス
/// </summary>
class DirectXDevice {
public:	// コンストラクタ

	DirectXDevice() = default;
	~DirectXDevice() = default;

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="useAdapter"></param>
	void Init(IDXGIAdapter4* useAdapter);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

public:

	ID3D12Device* GetDevice() { return device_.Get(); }

private:

	ComPtr<ID3D12Device> device_ = nullptr;

};