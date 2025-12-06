#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"
#include "Engine/Lib/Math/Vector2.h"

namespace PostEffect {

/// <summary>
/// 横幅のブラー
/// </summary>
class GaussianBlurWidth :
	public IPostEffect {
public:	// 構造体

	struct BlurSettings {
		Math::Vector2 texelSize;
	};

public:

	GaussianBlurWidth() = default;
	~GaussianBlurWidth() override;

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

	AOENGINE::DxResource* blurBuffer_;
	BlurSettings* blurSetting_;

};

}