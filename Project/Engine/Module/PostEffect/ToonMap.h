#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

class ToonMap :
	public IPostEffect {
public:

	ToonMap() = default;
	~ToonMap() override = default;

	void Init() override;

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;

	void CheckBox() override;

	void Debug_Gui() override {};
};

