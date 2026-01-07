#pragma once
#include "Engine/Lib/Color.h"
#include "Engine/Module/PostEffect/IPostEffect.h"

namespace PostEffect {

/// <summary>
/// ヴィネット
/// </summary>
class Vignette :
	public IPostEffect {
public:	// 構造体

	struct VignetteSetting {
		AOENGINE::Color color;
		float scale = 16.0f;
		float power = 0.8f;
	};

public:

	Vignette() = default;
	~Vignette() override;

public:

	// 初期化
	void Init() override;
	// コマンドの追加
	void SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) override;
	// チェックボックスの表示
	void CheckBox() override;
	// 編集処理
	void Debug_Gui() override;

public:

	void SetPower(float _power) { setting_->power = _power; }

	void SetScale(float _scale) { setting_->scale = _scale; }

	void SetColor(const AOENGINE::Color& _color) { setting_->color = _color; }

private:

	AOENGINE::DxResource* settingBuffer_;
	VignetteSetting* setting_;

};
}
