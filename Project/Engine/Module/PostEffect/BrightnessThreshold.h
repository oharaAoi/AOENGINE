#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

class BrightnessThreshold :
	public IPostEffect {
public:	// 構造体

	struct BloomSettings {
		float threshold = 1.0f;
	};

public:

	BrightnessThreshold() = default;
	~BrightnessThreshold() override;

	void Init() override;

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;

	void CheckBox() override;

	void Debug_Gui() override;

private:

	std::unique_ptr<DxResource> bloomBuffer_;
	BloomSettings* bloomSetting_;

};

