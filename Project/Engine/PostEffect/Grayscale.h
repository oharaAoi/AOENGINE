#pragma once
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Resource/DxResource.h"

class Grayscale {
public:

	Grayscale() =default;
	~Grayscale();

	void Init();

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource);

private:

	std::unique_ptr<DxResource> resource_;

};

