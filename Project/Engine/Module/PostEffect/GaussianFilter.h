#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

namespace PostEffect {

/// <summary>
/// BoxFilter
/// </summary>
class GaussianFilter :
	public IPostEffect {
public:

	struct Setting {
		float deviation;
		uint32_t size;
	};

public:

	GaussianFilter() = default;
	~GaussianFilter() override;

public:

	// 初期化
	void Init();
	// コマンドを積む
	void SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) override;
	// チェックボックスの表示
	void CheckBox() override;
	// 編集処理
	void Debug_Gui() override {};

private:

	AOENGINE::DxResource* settingBuffer_;
	Setting* setting_;

};

}