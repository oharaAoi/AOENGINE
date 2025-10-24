#pragma once
#include "Engine/Lib/Color.h"
#include "Engine/Module/PostEffect/IPostEffect.h"

/// <summary>
/// ヴィネット
/// </summary>
class Vignette :
	public IPostEffect {
public:	// 構造体

	struct VignetteSetting {
		Color color;
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
	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;
	// チェックボックスの表示
	void CheckBox() override;
	// 編集処理
	void Debug_Gui() override;

public:

	void SetPower(float _power) { setting_->power = _power; }

private:

	std::unique_ptr<DxResource> settingBuffer_;
	VignetteSetting* setting_;

};

