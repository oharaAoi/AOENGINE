#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

class MotionBlur :
	public IPostEffect {
public:

	MotionBlur() = default;
	~MotionBlur() override;

	void Init() override;

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;

	void SetMotionResource(DxResource* _resource) { motionResource_ = _resource; }

	void CheckBox() override;

	void Debug_Gui() override {};

private:

	DxResource* motionResource_;

};

