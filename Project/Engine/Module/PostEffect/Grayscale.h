#pragma once
#include "Engine/Lib/Color.h"
#include "Engine/Module/PostEffect/IPostEffect.h"

/// <summary>
/// 画面全体を灰色にする
/// </summary>
class Grayscale :
	public IPostEffect {
public:

	struct Setting {
		Color color;
	};

public:

	Grayscale() =default;
	~Grayscale() override;

public:

	// 初期化
	void Init() override;
	// コマンドを積む
	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;
	// チェックボックスを表示
	void CheckBox() override;
	// 編集処理
	void Debug_Gui() override;

public:

	void SetColor(const Color& _color) { setting_->color = _color; }

private:

	DxResource* settingBuffer_;
	Setting* setting_;

};

