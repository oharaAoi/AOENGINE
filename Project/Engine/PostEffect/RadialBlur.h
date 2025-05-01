#pragma once
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Resource/DxResource.h"

/// <summary>
/// 放射ブラー
/// </summary>
class RadialBlur {
public:

	struct BlurSetting {
		Vector2 blurCenter = {0.5f, 0.5f};	// 放射ブラーの中心（通常はfloat2(0.5, 0.5)）
		float blurStrength = 0.02f;			// ブラーの強度（例: 0.02）
		float blurStart = 0.2f;				// ブラーの開始距離
		int sampleCount = 16;				// サンプル数（例: 16）
	};

public:

	RadialBlur() = default;
	~RadialBlur();

	void Init();

	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource);

#ifdef _DEBUG
	void Debug_Gui();
#endif

private:

	std::unique_ptr<DxResource> blurSettingBuffer_;
	BlurSetting* setting_;

};

