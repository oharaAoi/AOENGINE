#pragma once
#include <list>
#include <memory>
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"

/// <summary>
/// DirectXのリソースを管理する
/// </summary>
class DxResourceManager {
public: // コンストラクタ

	DxResourceManager() = default;
	~DxResourceManager();

public:

	void Finalize();

	void Init(ID3D12Device* _device, DescriptorHeap* _dxHeap);

	void Update();

	DxResource* CreateResource(ResourceType _type);

private:

	ID3D12Device* device_;
	DescriptorHeap* dxHeap_;
	std::list<std::unique_ptr<DxResource>> resourceList_;

};

