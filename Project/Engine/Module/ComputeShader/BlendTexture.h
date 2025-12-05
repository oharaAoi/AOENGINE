#pragma once
#include <memory>
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/DirectX/Resource/DxResourceManager.h"
#include "Engine/DirectX/Pipeline/Pipeline.h"

namespace AOENGINE {

/// <summary>
/// resourceの合成
/// </summary>
class BlendTexture {
public:

	BlendTexture();
	~BlendTexture();

public:

	// 初期化
	void Init(AOENGINE::DxResourceManager* _dxManager);
	// 実行
	void Execute(AOENGINE::Pipeline* _pipeline, ID3D12GraphicsCommandList* commandList, const D3D12_GPU_DESCRIPTOR_HANDLE& refarebceGpuHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& rendrerGpuHandle);

private:

	AOENGINE::DxResource* resultResource_ = nullptr;

	UINT groupCountX_;
	UINT groupCountY_;

};

}