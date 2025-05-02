#pragma once
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/DirectX/Resource/DxResource.h"

/// <summary>
/// グリッチエフェクト
/// </summary>
class GlitchNoise {
public:

	struct GlitchSetting {
		Vector2 texelSize;	// texelのサイズ
		float time;			// 時間
		float strength;
		int frameIndex;
	};

public:

	GlitchNoise() = default;
	~GlitchNoise();

	void Init();

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource);

	void StartNoise(float startStrength, float time);

#ifdef _DEBUG
	void Debug_Gui();
#endif

private:

	std::unique_ptr<DxResource> glitchBuffer_;
	GlitchSetting* setting_;

	float noiseTime_;
};

