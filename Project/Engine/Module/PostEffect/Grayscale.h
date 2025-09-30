#pragma once
#include "Engine/Lib/Color.h"
#include "Engine/Module/PostEffect/IPostEffect.h"

class Grayscale :
	public IPostEffect {
public:

	struct Setting {
		Color color;
	};

public:

	Grayscale() =default;
	~Grayscale() override;

	void Init() override;

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;

	void CheckBox() override;

	void Debug_Gui() override;

public:

	void SetColor(const Color& _color) { setting_->color = _color; }

private:

	std::unique_ptr<DxResource> settingBuffer_;
	Setting* setting_;

};

