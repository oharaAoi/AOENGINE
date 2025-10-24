#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

/// <summary>
/// モーションブラー
/// </summary>
class MotionBlur :
	public IPostEffect {
public:

	MotionBlur() = default;
	~MotionBlur() override;

public:

	// 初期化
	void Init() override;
	// コマンドを積む
	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;
	// チェックボックスの表示
	void CheckBox() override;
	// 編集処理
	void Debug_Gui() override {};

public:

	void SetMotionResource(DxResource* _resource) { motionResource_ = _resource; }

private:

	DxResource* motionResource_;

};

