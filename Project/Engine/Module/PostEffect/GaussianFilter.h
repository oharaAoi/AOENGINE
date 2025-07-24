#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

class GaussianFilter :
	public IPostEffect {
public:

	struct Setting {
		float deviation;
		uint32_t size;
	};

public:

	GaussianFilter() = default;
	~GaussianFilter() override;

	void Init();

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;

	void CheckBox() override;

	void Debug_Gui() override {};

private:

	std::unique_ptr<DxResource> settingBuffer_;
	Setting* setting_;


};

