#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

namespace PostEffect {

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
	void SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) override;
	// チェックボックスの表示
	void CheckBox() override;
	// 編集処理
	void Debug_Gui() override {};

public:

	void SetMotionResource(AOENGINE::DxResource* _resource) { motionResource_ = _resource; }

private:

	AOENGINE::DxResource* motionResource_;

};

}