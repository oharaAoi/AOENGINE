#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

/// <summary>
/// 輝度ベースアウトライン
/// </summary>
class LuminanceBasedOutline :
	public IPostEffect {
public:

	LuminanceBasedOutline() = default;
	~LuminanceBasedOutline() override = default;

public:

	// 初期化
	void Init() override;
	// コマンドを積む
	void SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) override;
	// チェックボックスの表示
	void CheckBox() override;
	// 編集処理
	void Debug_Gui() override {};

};

