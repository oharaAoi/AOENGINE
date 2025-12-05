#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"
#include "Engine/Lib/Color.h"

/// <summary>
/// ディゾルブ
/// </summary>
class Dissolve :
	public IPostEffect {
public:

	struct DissolveSetting {
		Matrix4x4 uvTransform;
		Color color;
		Color edgeColor;
		float threshold = 0.5f;
	};

public:

	Dissolve() = default;
	~Dissolve() override;

public:

	// 初期化
	void Init() override;
	// コマンドを積む
	void SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) override;
	// チェックボックスの表示
	void CheckBox() override;
	// 編集項目
	void Debug_Gui() override;

private:

	AOENGINE::DxResource* settingBuffer_;
	DissolveSetting* setting_;

};

