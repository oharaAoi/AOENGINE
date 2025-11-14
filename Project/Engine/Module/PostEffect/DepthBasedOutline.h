#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"
#include "Engine/Lib/Math/Matrix4x4.h"

/// <summary>
/// 深度ベースアウトライン
/// </summary>
class DepthBasedOutline :
	public IPostEffect {
public:

	struct Setting {
		Matrix4x4 projectionInverse;
		float edgeGain;
	};

public:

	DepthBasedOutline() = default;
	~DepthBasedOutline() override;

public:

	// 初期化
	void Init() override;
	// コマンドを積む
	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;
	// チェックボックス
	void CheckBox() override;
	// 編集書処理
	void Debug_Gui() override;

private:

	DxResource* settingBuffer_;
	Setting* setting_;

	ID3D12Resource* depthResource_;
	DescriptorHandles depthHandle_;
};

