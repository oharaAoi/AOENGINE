#pragma once
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

class ShadowMap :
	public AttributeGui {
public:

	ShadowMap() = default;
	~ShadowMap();

	void Init();

	void SetCommand();

	void ChangeResource(ID3D12GraphicsCommandList* commandList);
	void ResetResource(ID3D12GraphicsCommandList* commandList);

	ID3D12Resource* GetDepthResource() { return depthStencilResource_->GetResource(); }

	DescriptorHandles GetDepthDsvHandle() { return depthDsvHandle_; }
	DescriptorHandles GetDeptSrvHandle() { return depthSrvHandle_; }

	void Debug_Gui() override;

private:

	std::unique_ptr<DxResource> depthStencilResource_ = nullptr;
	DescriptorHandles depthDsvHandle_;
	DescriptorHandles depthSrvHandle_;

};