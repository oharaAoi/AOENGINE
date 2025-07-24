#pragma once
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Module/PostEffect/IPostEffect.h"

/// <summary>
/// グリッチエフェクト
/// </summary>
class GlitchNoise :
	public IPostEffect {
public:

	struct GlitchSetting {
		Vector2 texelSize;	// texelのサイズ
		float time;			// 時間
		float strength;
		int frameIndex;
	};

public:

	GlitchNoise() = default;
	~GlitchNoise() override ;

	void Init() override;

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;

	void StartNoise(float startStrength, float time);

	void CheckBox() override;

	void Debug_Gui() override;

private:

	std::unique_ptr<DxResource> glitchBuffer_;
	GlitchSetting* setting_;

	float noiseTime_;
};

