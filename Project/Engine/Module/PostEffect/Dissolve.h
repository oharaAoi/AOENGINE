#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"

class Dissolve :
	public IPostEffect {
public:

	struct DissolveSetting {
		float threshold = 0.5f;
	};

public:

	Dissolve() = default;
	~Dissolve() override;

	void Init() override;

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;

#ifdef _DEBUG
	void Debug_Gui();
#endif

private:

	std::unique_ptr<DxResource> settingBuffer_;
	DissolveSetting* setting_;

};

