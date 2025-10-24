#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

/// <summary>
/// Smoothing処理
/// </summary>
class Smoothing :
	public IPostEffect {
public:

	struct Setting {
		uint32_t size;
	};

public:

	Smoothing() = default;
	~Smoothing() override;

public:

	// 初期化
	void Init() override;
	// コマンドを積む
	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;
	// チェックボックスの表示
	void CheckBox() override;
	// 編集処理
	void Debug_Gui() override {};

private:

	std::unique_ptr<DxResource> settingBuffer_;
	Setting* setting_;

};

