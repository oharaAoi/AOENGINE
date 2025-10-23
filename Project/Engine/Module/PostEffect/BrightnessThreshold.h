#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

/// <summary>
/// 閾値以上の輝度を抽出
/// </summary>
class BrightnessThreshold :
	public IPostEffect {
public:	// 構造体

	struct BloomSettings {
		float threshold = 1.0f;
	};

public:

	BrightnessThreshold() = default;
	~BrightnessThreshold() override;

public:

	// 初期化
	void Init() override;
	// コマンドを積む
	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;

	void CheckBox() override;

	void Debug_Gui() override;

private:

	std::unique_ptr<DxResource> bloomBuffer_;
	BloomSettings* bloomSetting_;

};

