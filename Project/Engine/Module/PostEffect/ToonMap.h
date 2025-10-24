#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

class ToonMap :
	public IPostEffect {
public:

	ToonMap() = default;
	~ToonMap() override = default;

public:

	// 初期化
	void Init() override;
	// コマンドを積む
	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;
	// チェックボックスの表示
	void CheckBox() override;
	// 編集処理
	void Debug_Gui() override {};
};

