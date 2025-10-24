#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"
#include "Engine/Lib/Math/Vector2.h"

/// <summary>
/// 縦幅のブラー
/// </summary>
class GaussianBlurHeight :
	public IPostEffect {
public:	// 構造体

	struct BlurSettings {
		Vector2 texelSize;
	};

public:

	GaussianBlurHeight() = default;
	~GaussianBlurHeight() override;

public:

	// 初期化
	void Init() override;
	// コマンドを積む
	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;
	// チェックボックスの表示
	void CheckBox() override;
	// 編集処理
	void Debug_Gui() override;

private:

	std::unique_ptr<DxResource> blurBuffer_;
	BlurSettings* blurSetting_;
};

