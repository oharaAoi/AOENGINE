#pragma once
#include <list>
#include "Engine/DirectX/Resource/DxResource.h"

/// <summary>
/// DirectXのresourceなどを解放する際にこのクラスに集約させる
/// </summary>
class DescriptorFreeCollector {
public: // コンストラクタ

	DescriptorFreeCollector() = default;
	~DescriptorFreeCollector();


public:

	void Free();

	void AddList(DxResource* _resource);

private:

	std::list<DxResource*> dxResourceList_;

};

