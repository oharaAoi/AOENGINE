#pragma once
// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <string>

class Blend {
public:

	enum BlendMode {
		// ブレンドなし
		ModeNone,
		// 通常αブレンド
		ModeNormal,
		// 加算
		ModeAdd,
		// 減算
		ModeSubtract,
		// 乗算
		ModeMultily,
		// 反転
		ModeScreen
	};

public:

	Blend();
	~Blend();

	void Init();

	D3D12_BLEND_DESC SetBlend(const BlendMode& mode);

	D3D12_BLEND_DESC SetBlend(const std::string& modeName);

	void SetNormalBlend(D3D12_BLEND_DESC& blendDesc);

	void SetAddBlend(D3D12_BLEND_DESC& blendDesc);

	void SetSubtractBlend(D3D12_BLEND_DESC& blendDesc);

	void SetMultiplyBlend(D3D12_BLEND_DESC& blendDesc);

	void SetScreenBlend(D3D12_BLEND_DESC& blendDesc);

private:



};

