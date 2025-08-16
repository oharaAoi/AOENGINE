#pragma once
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
#include <DirectX/Resource/DxResource.h>

class ShadowMap {
public:

	ShadowMap() = default;
	~ShadowMap();

	void Init();

	void DrawDepth();

	void SetCommand();

	void ChangeResource(ID3D12GraphicsCommandList* commandList);
	void ResetResource(ID3D12GraphicsCommandList* commandList);

	ID3D12Resource* GetDepthResource() { return depthStencilResource_->GetResource(); }

	DescriptorHandles GetDepthDsvHandle() { return depthDsvHandle_; }
	DescriptorHandles GetDeptSrvHandle() { return depthSrvHandle_; }

private:

	std::unique_ptr<DxResource> depthStencilResource_ = nullptr;
	DescriptorHandles depthDsvHandle_;
	DescriptorHandles depthSrvHandle_;

};