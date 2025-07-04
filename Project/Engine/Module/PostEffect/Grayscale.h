#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

class Grayscale :
	public IPostEffect {
public:

	Grayscale() =default;
	~Grayscale() override;

	void Init() override;

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;

private:

};

