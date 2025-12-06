#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <string>
// DirectX
#include "Engine/DirectX/Utilities/DirectXUtils.h"
// math
#include "Engine/Lib/Math/MyMath.h"

namespace AOENGINE {

/// <summary>
/// gpuに送る情報
/// </summary>
struct ViewProjectionData {
	Math::Matrix4x4 view;
	Math::Matrix4x4 projection;
};

/// <summary>
/// 前フレーム分の情報
/// </summary>
struct ViewProjectionPrevData {
	Math::Matrix4x4 view;
	Math::Matrix4x4 projection;
};

/// <summary>
/// 透視投影行列
/// </summary>
class ViewProjection {
public:

	ViewProjection();
	~ViewProjection();

public:

	// 終了
	void Finalize();
	// 初期化
	void Init(ID3D12Device* device);
	// コマンドを積む
	void BindCommandList(ID3D12GraphicsCommandList* commandList, UINT index) const;
	void BindCommandListPrev(ID3D12GraphicsCommandList* commandList, UINT index) const;

	/// <summary>
	/// データをセットする
	/// </summary>
	/// <param name="view"></param>
	/// <param name="projection"></param>
	void SetViewProjection(const Math::Matrix4x4& view, const Math::Matrix4x4& projection) {
		dataPrev_->view = data_->view;
		dataPrev_->projection = data_->projection;
		data_->view = view;
		data_->projection = projection;
	}

	const Math::Matrix4x4 GetViewProjection() { return data_->view * data_->projection; }

	const Math::Matrix4x4 GetViewMatrix() { return data_->view; }
	const Math::Matrix4x4 GetProjectionMatrix() { return data_->projection; }

private:

	ComPtr<ID3D12Resource> cBuffer_;
	ViewProjectionData* data_;

	ComPtr<ID3D12Resource> cBufferPrev_;
	ViewProjectionPrevData* dataPrev_;
};

}