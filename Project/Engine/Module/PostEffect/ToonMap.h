#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

namespace PostEffect {

class ToonMap :
	public IPostEffect {
public:

	struct Parameter {
		float exposure;
	};

public:

	ToonMap() = default;
	~ToonMap() override = default;

public:

	// 初期化
	void Init() override;
	// コマンドを積む
	void SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) override;
	// チェックボックスの表示
	void CheckBox() override;
	// 編集処理
	void Debug_Gui() override;

private:

	AOENGINE::DxResource* settingBuffer_;
	Parameter* setting_;

};

}