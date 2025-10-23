#pragma once
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <string>
// DirectX
#include "Engine/DirectX/Utilities/DirectXUtils.h"
// math
#include "Engine/Lib/Math/MyMath.h"

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr <T>;

/// <summary>
/// Transformをgpuに送るクラス
/// </summary>
class TransformationMatrix {
public:

	struct TransformationData {
		Matrix4x4 world;
		Matrix4x4 view;
		Matrix4x4 projection;
		Matrix4x4 worldInverseTranspose;
	};

public:

	TransformationMatrix();
	~TransformationMatrix();

public:

	// 終了
	void Finalize();
	// 初期化
	void Init(ID3D12Device* device, const uint32_t& instanceSize);
	// 更新
	void Update(const Matrix4x4& world, const Matrix4x4& view, const Matrix4x4& projection);
	// コマンドを積む
	void Draw(ID3D12GraphicsCommandList* commandList);

public:

	TransformationData* GetTransformationData() { return TransformationData_; }

private:

	uint32_t instanceSize_;

	ComPtr<ID3D12Resource> cBuffer_;
	TransformationData* TransformationData_;
};