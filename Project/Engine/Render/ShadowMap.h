#pragma once
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// 影のレンダリングを行うクラス
/// </summary>
class ShadowMap :
	public AttributeGui {
public:

	ShadowMap() = default;
	~ShadowMap();

public:

	// 初期化
	void Init();
	// コマンドを積む
	void SetCommand();
	// resourceの変更
	void ChangeResource(ID3D12GraphicsCommandList* commandList);
	void ResetResource(ID3D12GraphicsCommandList* commandList);
	// 編集処理
	void Debug_Gui() override;
public:

	ID3D12Resource* GetDepthResource() { return depthStencilResource_->GetResource(); }

	DescriptorHandles GetDepthDsvHandle() { return depthDsvHandle_; }
	DescriptorHandles GetDeptSrvHandle() { return depthSrvHandle_; }

private:

	std::unique_ptr<DxResource> depthStencilResource_ = nullptr;
	DescriptorHandles depthDsvHandle_;
	DescriptorHandles depthSrvHandle_;

};