#pragma once
#include <memory>
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/DirectX/Pipeline/Pipeline.h"

class BlendTexture {
public:

	BlendTexture();
	~BlendTexture();

	void Init(ID3D12Device* device, DescriptorHeap* dxHeap);

	void Execute(Pipeline* _pipeline, ID3D12GraphicsCommandList* commandList, const D3D12_GPU_DESCRIPTOR_HANDLE& refarebceGpuHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& rendrerGpuHandle);

private:

	std::unique_ptr<DxResource> resultResource_;

	UINT groupCountX_;
	UINT groupCountY_;

};

