#pragma once
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Module/PostEffect/IPostEffect.h"

class Vignette :
	public IPostEffect {
public:	// 構造体

	struct VignetteSetting {
		Vector4 color;
		float scale = 16.0f;
		float power = 0.8f;
	};

public:

	Vignette() = default;
	~Vignette() override;

	void Init() override;

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;

	void CheckBox() override;

	void Debug_Gui() override;

	void SetPower(float _power) { setting_->power = _power; }

private:

	std::unique_ptr<DxResource> settingBuffer_;
	VignetteSetting* setting_;

};

