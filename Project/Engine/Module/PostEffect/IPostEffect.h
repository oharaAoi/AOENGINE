#pragma once
#include "Engine/DirectX/Resource/DxResource.h"

class IPostEffect {
public:

	IPostEffect() = default;
	virtual ~IPostEffect() = default;

	virtual void Init() = 0;

	virtual void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) = 0;

};

