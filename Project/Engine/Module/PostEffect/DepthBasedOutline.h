#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"
#include "Engine/Lib/Math/Matrix4x4.h"

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

	void Init() override;

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;

	void CheckBox() override;

	void Debug_Gui() override;

private:

	std::unique_ptr<DxResource> settingBuffer_;
	Setting* setting_;

	ID3D12Resource* depthResource_;
	DescriptorHandles depthHandle_;
};

