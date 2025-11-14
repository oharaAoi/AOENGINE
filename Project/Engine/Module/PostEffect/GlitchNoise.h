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

public:

	// 初期化
	void Init() override;
	// コマンドを積む
	void SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) override;
	// チェックボックスの表示
	void CheckBox() override;
	// 編集処理
	void Debug_Gui() override;

	/// <summary>
	/// Noiseを実行させる
	/// </summary>
	/// <param name="startStrength">: 開始の強さ</param>
	/// <param name="time">: ノイズの時間</param>
	void StartNoise(float startStrength, float time);

private:

	DxResource* glitchBuffer_;
	GlitchSetting* setting_;

	float noiseTime_;
};

