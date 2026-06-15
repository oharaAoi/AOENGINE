#pragma once
#include <list>
#include <memory>
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"

namespace AOENGINE {

/// <summary>
/// DirectXのリソースを管理する
/// </summary>
class DxResourceManager {
public: // コンストラクタ

	DxResourceManager() = default;
	~DxResourceManager() = default;;

public:

	void Finalize();

	void Init(ID3D12Device* _device, AOENGINE::DescriptorHeap* _dxHeap);

	void Update();

	AOENGINE::DxResource* CreateResource(ResourceType _type);

private:

	ID3D12Device* device_;
	AOENGINE::DescriptorHeap* dxHeap_;
	std::list<std::unique_ptr<AOENGINE::DxResource>> resourceList_;

};

}