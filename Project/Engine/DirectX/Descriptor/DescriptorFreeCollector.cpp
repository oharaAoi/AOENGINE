#include "DescriptorFreeCollector.h"

DescriptorFreeCollector::~DescriptorFreeCollector() {
	dxResourceList_.clear();
}


void DescriptorFreeCollector::Free() {
	for (auto& resource : dxResourceList_) {
		resource->Finalize();
	}
	dxResourceList_.clear();
}

void DescriptorFreeCollector::AddList(DxResource* _resource) {
	dxResourceList_.push_back(_resource);
}
