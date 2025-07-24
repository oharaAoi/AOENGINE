#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"
#include "Engine/Lib/Math/Vector4.h"

class Dissolve :
	public IPostEffect {
public:

	struct DissolveSetting {
		Matrix4x4 uvTransform;
		Vector4 color;
		Vector4 edgeColor;
		float threshold = 0.5f;
	};

public:

	Dissolve() = default;
	~Dissolve() override;

	void Init() override;

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;

	void CheckBox() override;

	void Debug_Gui() override;

private:

	std::unique_ptr<DxResource> settingBuffer_;
	DissolveSetting* setting_;

};

