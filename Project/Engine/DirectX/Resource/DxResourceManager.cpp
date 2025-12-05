#include "DxResourceManager.h"
#include "Engine/Utilities/Logger.h"

using namespace AOENGINE;

void DxResourceManager::Finalize() {
	resourceList_.clear();
}

void DxResourceManager::Init(ID3D12Device* _device, AOENGINE::DescriptorHeap* _dxHeap) {
	device_ = _device;
	dxHeap_ = _dxHeap;
}

void DxResourceManager::Update() {
	std::erase_if(resourceList_, [](const std::unique_ptr<AOENGINE::DxResource>& resource) {
		return resource->GetIsDestroy();
				  });
}

DxResource* DxResourceManager::CreateResource(ResourceType _type) {
	auto& resource = resourceList_.emplace_back(std::make_unique<AOENGINE::DxResource>());
	resource->Init(device_, dxHeap_, _type);
	if (!resource) {
		Logger::AssertLog("Don`t Create AOENGINE::DxResource");
	}
	return resource.get();
}
