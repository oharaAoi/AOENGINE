#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"
#include "Engine/Lib/Math/Vector2.h"

class GaussianBlurWidth :
	public IPostEffect {
public:	// 構造体

	struct BlurSettings {
		Vector2 texelSize;
	};

public:

	GaussianBlurWidth() = default;
	~GaussianBlurWidth() override;

	void Init() override;

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;

	void CheckBox() override;

	void Debug_Gui() override;

private:

	std::unique_ptr<DxResource> blurBuffer_;
	BlurSettings* blurSetting_;

};

