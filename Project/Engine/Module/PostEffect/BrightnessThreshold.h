#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

namespace PostEffect {

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
	void SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) override;
	// チェックボックスの表示
	void CheckBox() override;
	// 編集処理
	void Debug_Gui() override;

private:

	AOENGINE::DxResource* bloomBuffer_;
	BloomSettings* bloomSetting_;

};

}