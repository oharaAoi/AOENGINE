#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

class Smoothing :
	public IPostEffect {
public:

	struct Setting {
		uint32_t size;
	};

public:

	Smoothing() = default;
	~Smoothing() override;

	void Init() override;

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;

	void CheckBox() override;

	void Debug_Gui() override {};

private:

	std::unique_ptr<DxResource> settingBuffer_;
	Setting* setting_;

};

